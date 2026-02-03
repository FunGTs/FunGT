#include "../include/gpu_radix_sort.hpp"
#include "gpu_radix_sort.hpp"

gpu::RadixSort::RadixSort(sycl::queue& queue)
    : m_queue(queue)
    , m_radixInitialized(false)
    , m_maxBodies(0)
{
    // Initialize pointers to nullptr
    m_radixData.keys_in = nullptr;
    m_radixData.keys_out = nullptr;
    m_radixData.values_in = nullptr;
    m_radixData.values_out = nullptr;
    m_radixData.radixCounts = nullptr;
    m_radixData.radixOffsets = nullptr;
    m_radixData.numThreadBlocks = 0;
    m_radixData.threadsPerBlock = 0;
}

gpu::RadixSort::~RadixSort() {
    cleanup();
}

void gpu::RadixSort::cleanup() {
    if (m_radixData.keys_in) sycl::free(m_radixData.keys_in, m_queue);
    if (m_radixData.keys_out) sycl::free(m_radixData.keys_out, m_queue);
    if (m_radixData.values_in) sycl::free(m_radixData.values_in, m_queue);
    if (m_radixData.values_out) sycl::free(m_radixData.values_out, m_queue);
    if (m_radixData.radixCounts) sycl::free(m_radixData.radixCounts, m_queue);
    if (m_radixData.radixOffsets) sycl::free(m_radixData.radixOffsets, m_queue);

    m_radixData.keys_in = nullptr;
    m_radixData.keys_out = nullptr;
    m_radixData.values_in = nullptr;
    m_radixData.values_out = nullptr;
    m_radixData.radixCounts = nullptr;
    m_radixData.radixOffsets = nullptr;

    m_radixInitialized = false;
}

void gpu::RadixSort::init(int maxBodies) {
    if (m_radixInitialized) {
        cleanup();
    }

    m_maxBodies = maxBodies;
    const int NUM_RADICES = 256;
    m_radixData.numThreadBlocks = 16;
    m_radixData.threadsPerBlock = 256;

    int totalThreads = m_radixData.numThreadBlocks * m_radixData.threadsPerBlock;

    // Allocate double buffers
    m_radixData.keys_in = sycl::malloc_device<int>(maxBodies, m_queue);
    m_radixData.keys_out = sycl::malloc_device<int>(maxBodies, m_queue);
    m_radixData.values_in = sycl::malloc_device<int>(maxBodies, m_queue);
    m_radixData.values_out = sycl::malloc_device<int>(maxBodies, m_queue);

    // Allocate radix counters
    m_radixData.radixCounts = sycl::malloc_device<int>(NUM_RADICES * totalThreads, m_queue);
    m_radixData.radixOffsets = sycl::malloc_device<int>(NUM_RADICES * totalThreads, m_queue);

    m_radixInitialized = true;
}

void gpu::RadixSort::sort(int* cellHash, int* bodyIndex, int numBodies) {
    if (!m_radixInitialized) {
        throw std::runtime_error("RadixSort not initialized!");
    }

    // Copy input data to internal buffers
    m_queue.memcpy(m_radixData.keys_in, cellHash, numBodies * sizeof(int)).wait();
    m_queue.memcpy(m_radixData.values_in, bodyIndex, numBodies * sizeof(int)).wait();

    // 4 passes for 32-bit integers (8 bits per pass)
    for (int pass = 0; pass < 4; pass++) {
        int bitShift = pass * 8;  // 0, 8, 16, 24
        radixSortPass(bitShift, numBodies);

        // Ping-pong buffers after each pass
        std::swap(m_radixData.keys_in, m_radixData.keys_out);
        std::swap(m_radixData.values_in, m_radixData.values_out);
    }

    // Copy sorted data back
    m_queue.memcpy(cellHash, m_radixData.keys_in, numBodies * sizeof(int)).wait();
    m_queue.memcpy(bodyIndex, m_radixData.values_in, numBodies * sizeof(int)).wait();
}
void gpu::RadixSort::radixSortPass(int bitShift, int numBodies)
{
    const int NUM_RADICES = 256;
    const int RADIX_MASK = 0xFF;

    RadixSortData radix = m_radixData;
    int blockSize = radix.threadsPerBlock;

    // 2D grid mapping 
    std::size_t xdim = 32;
    std::size_t ydim = (numBodies + xdim - 1) / xdim;

    // Work-group size (local size) - 2D
    std::size_t local_x = 16;
    std::size_t local_y = 16;  // 16x16 = 256 threads per work-group

    // Global size - round up to multiple of local size
    std::size_t global_x = ((xdim + local_x - 1) / local_x) * local_x;
    std::size_t global_y = ((ydim + local_y - 1) / local_y) * local_y;

    // Phase 1: Count radices per work-group
    m_queue.submit([&](sycl::handler& cgh) {
        cgh.parallel_for(
            sycl::nd_range<2>(
                sycl::range<2>(global_y, global_x),  // Global range
                sycl::range<2>(local_y, local_x)     // Work-group size
            ),
            [=](sycl::nd_item<2> item) {

                // 2D indexing
                std::size_t i = item.get_global_id(0) * xdim + item.get_global_id(1);
                if (i >= numBodies) return;

                // Local thread IDs
                int local_tid = item.get_local_id(0) * local_x + item.get_local_id(1);
                int group_id = item.get_group(0) * (global_x / local_x) + item.get_group(1);

                // Shared memory for local counting
                auto localCounts = sycl::ext::oneapi::group_local_memory<int[NUM_RADICES]>(item.get_group());

                // Initialize local counts
                if (local_tid < NUM_RADICES) {
                    localCounts[local_tid] = 0;
                }
                item.barrier(sycl::access::fence_space::local_space);

                // Count this thread's element
                int key = radix.keys_in[i];
                int radixValue = (key >> bitShift) & RADIX_MASK;

                // Atomic increment in shared memory
                sycl::atomic_ref<int, sycl::memory_order::relaxed, sycl::memory_scope::work_group>
                    atomic_count(localCounts[radixValue]);
                atomic_count.fetch_add(1);

                item.barrier(sycl::access::fence_space::local_space);

                // Write block's counts to global memory (first 256 threads do this)
                if (local_tid < NUM_RADICES) {
                    radix.radixCounts[group_id * NUM_RADICES + local_tid] = localCounts[local_tid];
                }
            });
        }).wait();

    // Phase 2: Compute prefix sum of counts (CPU for now)
    int numGroups = (global_y / local_y) * (global_x / local_x);
    std::vector<int> counts_host(NUM_RADICES * numGroups);
    m_queue.memcpy(counts_host.data(), radix.radixCounts, NUM_RADICES * numGroups * sizeof(int)).wait();

    // Sum across all groups for each radix
    std::vector<int> totalCounts(NUM_RADICES, 0);
    for (int r = 0; r < NUM_RADICES; r++) {
        for (int g = 0; g < numGroups; g++) {
            totalCounts[r] += counts_host[g * NUM_RADICES + r];
        }
    }

    // Compute global prefix sum
    std::vector<int> globalOffsets(NUM_RADICES, 0);
    for (int i = 1; i < NUM_RADICES; i++) {
        globalOffsets[i] = globalOffsets[i - 1] + totalCounts[i - 1];
    }

    // Compute per-group offsets
    std::vector<int> offsets_host(NUM_RADICES * numGroups);
    for (int g = 0; g < numGroups; g++) {
        for (int r = 0; r < NUM_RADICES; r++) {
            if (g == 0) {
                offsets_host[g * NUM_RADICES + r] = globalOffsets[r];
            }
            else {
                offsets_host[g * NUM_RADICES + r] = offsets_host[(g - 1) * NUM_RADICES + r] + counts_host[(g - 1) * NUM_RADICES + r];
            }
        }
    }

    m_queue.memcpy(radix.radixOffsets, offsets_host.data(), NUM_RADICES * numGroups * sizeof(int)).wait();

    // Phase 3: Reorder elements
    m_queue.submit([&](sycl::handler& cgh) {
        cgh.parallel_for(
            sycl::nd_range<2>(
                sycl::range<2>(global_y, global_x),
                sycl::range<2>(local_y, local_x)
            ),
            [=](sycl::nd_item<2> item) {

                std::size_t i = item.get_global_id(0) * xdim + item.get_global_id(1);
                if (i >= numBodies) return;

                int local_tid = item.get_local_id(0) * local_x + item.get_local_id(1);
                int group_id = item.get_group(0) * (global_x / local_x) + item.get_group(1);

                // Shared memory for group-local offsets
                auto localOffsets = sycl::ext::oneapi::group_local_memory<int[NUM_RADICES]>(item.get_group());

                // Load offsets for this group
                if (local_tid < NUM_RADICES) {
                    localOffsets[local_tid] = radix.radixOffsets[group_id * NUM_RADICES + local_tid];
                }
                item.barrier(sycl::access::fence_space::local_space);

                // Get this element's radix
                int key = radix.keys_in[i];
                int value = radix.values_in[i];
                int radixValue = (key >> bitShift) & RADIX_MASK;

                // Atomically get output position
                sycl::atomic_ref<int, sycl::memory_order::relaxed, sycl::memory_scope::work_group>
                    atomic_offset(localOffsets[radixValue]);
                int outputPos = atomic_offset.fetch_add(1);

                // Write to output
                radix.keys_out[outputPos] = key;
                radix.values_out[outputPos] = value;
            });
        }).wait();
}