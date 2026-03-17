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

    //std::cout << "RadixSort::sort() called with numBodies=" << numBodies << "\n";

    // Copy input data to internal buffers
    //std::cout << "Copying input to device...\n";
    m_queue.memcpy(m_radixData.keys_in, cellHash, numBodies * sizeof(int)).wait();
    m_queue.memcpy(m_radixData.values_in, bodyIndex, numBodies * sizeof(int)).wait();
    //std::cout << "Input copied.\n";

    // 4 passes for 32-bit integers (8 bits per pass)
    for (int pass = 0; pass < 4; pass++) {
        //std::cout << "Starting pass " << pass << "...\n";
        int bitShift = pass * 8;

        try {
            radixSortPass(bitShift, numBodies);
            //std::cout << "Pass " << pass << " completed.\n";
        }
        catch (const sycl::exception& e) {
            std::cerr << "CRASH in pass " << pass << ": " << e.what() << "\n";
            throw;
        }

        // Ping-pong buffers after each pass
        std::swap(m_radixData.keys_in, m_radixData.keys_out);
        std::swap(m_radixData.values_in, m_radixData.values_out);
    }

    //std::cout << "All passes done. Copying back...\n";
    // Copy sorted data back
    m_queue.memcpy(cellHash, m_radixData.keys_in, numBodies * sizeof(int)).wait();
    m_queue.memcpy(bodyIndex, m_radixData.values_in, numBodies * sizeof(int)).wait();
    //std::cout << "RadixSort complete!\n";
}
void gpu::RadixSort::radixSortPass(int bitShift, int numBodies)
{
    //std::cout << "  radixSortPass bitShift=" << bitShift << " numBodies=" << numBodies << "\n";
    const int NUM_RADICES = 256;
    const int RADIX_MASK = 0xFF;

    RadixSortData radix = m_radixData;

    // 2D grid mapping 
    std::size_t xdim = 32;
    std::size_t ydim = (numBodies + xdim - 1) / xdim;

    // Work-group size (local size) - 2D
    std::size_t local_x = 16;
    std::size_t local_y = 16;

    // Global size - round up to multiple of local size
    std::size_t global_x = ((xdim + local_x - 1) / local_x) * local_x;
    std::size_t global_y = ((ydim + local_y - 1) / local_y) * local_y;

    
    // std::cout << "  Grid: global=" << global_y << "x" << global_x
    //     << " local=" << local_y << "x" << local_x << "\n";

    // // Phase 1: Count
    // std::cout << "  Phase 1: Counting...\n";
    // auto device = m_queue.get_device();
    // std::cout << "GPU Physics Radix Sort on: "
    //     << device.get_info<sycl::info::device::name>()
    //     << std::endl;


    try {
        m_queue.submit([&](sycl::handler& cgh) {
            auto localCounts = sycl::local_accessor<int, 1>(NUM_RADICES, cgh);

            cgh.parallel_for(
                sycl::nd_range<2>(
                    sycl::range<2>(global_y, global_x),
                    sycl::range<2>(local_y, local_x)
                ),
                [=](sycl::nd_item<2> item) {

                    // FIX: Correct 2D to 1D mapping
                    std::size_t row = item.get_global_id(0);
                    std::size_t col = item.get_global_id(1);
                    std::size_t i = row * xdim + col;
        
                    // Local thread IDs
                    int local_tid = item.get_local_linear_id();
                    int num_groups_x = global_x / local_x;
                    int group_id = item.get_group(0) * num_groups_x + item.get_group(1);

                    // Initialize local counts
                    if (local_tid < NUM_RADICES) {
                        localCounts[local_tid] = 0;
                    }
                    item.barrier(sycl::access::fence_space::local_space);

                    // NOW check bounds (some threads will skip counting)
                    if (i < numBodies) {
                        int key = radix.keys_in[i];
                        //if (key >= 0) {
                            int radixValue = (key >> bitShift) & RADIX_MASK;

                            sycl::atomic_ref<int, sycl::memory_order::relaxed, sycl::memory_scope::work_group>
                                atomic_count(localCounts[radixValue]);
                            atomic_count.fetch_add(1);
                        //}
                    }

                    // BARRIER 2: Wait for counting (ALL threads reach here)
                    item.barrier(sycl::access::fence_space::local_space);

                    // Write results
                    if (local_tid < NUM_RADICES) {
                        radix.radixCounts[group_id * NUM_RADICES + local_tid] = localCounts[local_tid];
                    }
                });
            }).wait();

        //std::cout << "  Phase 1 done.\n";
    }
    catch (const sycl::exception& e) {
        std::cerr << "  CRASH in Phase 1: " << e.what() << "\n";
        throw;
    }
    
    // ===============================
 // Phase 2: Prefix sum (GPU)
 // ===============================


    int num_groups_x = global_x / local_x;
    int num_groups_y = global_y / local_y;
    int numGroups = num_groups_x * num_groups_y;

    // --------------------------------------------------
    // Step 1: totalCounts[r] = sum_g radixCounts[g][r]
    // --------------------------------------------------

    int* totalCounts = sycl::malloc_device<int>(NUM_RADICES, m_queue);
    m_queue.fill(totalCounts, 0, NUM_RADICES).wait();

    m_queue.submit([&](sycl::handler& cgh) {
        cgh.parallel_for(sycl::range<1>(NUM_RADICES), [=](sycl::id<1> r) {
            int sum = 0;
            for (int g = 0; g < numGroups; g++) {
                sum += radix.radixCounts[g * NUM_RADICES + r];
            }
            totalCounts[r] = sum;
            });
        }).wait();

    // --------------------------------------------------
    // Step 2: globalOffsets[r] = exclusive scan of totals
    // --------------------------------------------------

    int* globalOffsets = sycl::malloc_device<int>(NUM_RADICES, m_queue);

    m_queue.submit([&](sycl::handler& cgh) {
        cgh.single_task([=]() {
            int running = 0;
            for (int r = 0; r < NUM_RADICES; r++) {
                globalOffsets[r] = running;
                running += totalCounts[r];
            }
            });
        }).wait();

    // --------------------------------------------------
    // Step 3: radixOffsets[g][r] = per-group prefix
    // --------------------------------------------------

    m_queue.submit([&](sycl::handler& cgh) {
        cgh.parallel_for(sycl::range<1>(NUM_RADICES), [=](sycl::id<1> r) {
            int offset = globalOffsets[r];

            for (int g = 0; g < numGroups; g++) {
                radix.radixOffsets[g * NUM_RADICES + r] = offset;
                offset += radix.radixCounts[g * NUM_RADICES + r];
            }
            });
        }).wait();

    // --------------------------------------------------
    // Cleanup
    // --------------------------------------------------

    sycl::free(totalCounts, m_queue);
    sycl::free(globalOffsets, m_queue);

    //std::cout << "  Phase 2 done.\n";
    // Phase 3: Reorder elements


    //std::cout << "  Phase 3: Reordering...\n";
    try {
        m_queue.submit([&](sycl::handler& cgh) {
            auto localOffsets = sycl::local_accessor<int, 1>(NUM_RADICES, cgh);

            cgh.parallel_for(
                sycl::nd_range<2>(
                    sycl::range<2>(global_y, global_x),
                    sycl::range<2>(local_y, local_x)
                ),
                [=](sycl::nd_item<2> item) {

                    std::size_t row = item.get_global_id(0);
                    std::size_t col = item.get_global_id(1);
                    std::size_t i = row * xdim + col;

                    int local_tid = item.get_local_linear_id();
                    int num_groups_x = global_x / local_x;
                    int group_id = item.get_group(0) * num_groups_x + item.get_group(1);

                    // Load offsets (ALL threads participate)
                    if (local_tid < NUM_RADICES) {
                        localOffsets[local_tid] = radix.radixOffsets[group_id * NUM_RADICES + local_tid];
                    }

                    // BARRIER: ALL threads must reach
                    item.barrier(sycl::access::fence_space::local_space);

                    // Only valid threads do reordering
                    if (i < numBodies) {
                        int key = radix.keys_in[i];
                        int value = radix.values_in[i];

                        //if (key < 0) {
                            radix.keys_out[i] = key;
                            radix.values_out[i] = value;
                        //}
                        //else {
                            int radixValue = (key >> bitShift) & RADIX_MASK;

                            sycl::atomic_ref<int, sycl::memory_order::relaxed, sycl::memory_scope::work_group>
                                atomic_offset(localOffsets[radixValue]);
                            int outputPos = atomic_offset.fetch_add(1);

                            radix.keys_out[outputPos] = key;
                            radix.values_out[outputPos] = value;
                        //}
                    }
                });
            }).wait();
        //std::cout << "  Phase 3 done.\n";
    }
    catch (const sycl::exception& e) {
        std::cerr << "  CRASH in Phase 3: " << e.what() << "\n";
        throw;
    }
  
}