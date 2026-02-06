#if !defined(_GPU_RADIX_SORT_H_)
#define _GPU_RADIX_SORT_H_
#include "gpu_includes.hpp"
namespace gpu {

    struct RadixSortData {
        // Sorting buffers (double buffered for ping-pong)
        int* keys_in;
        int* keys_out;
        int* values_in;
        int* values_out;

        // Per-radix counters (256 buckets × numThreadBlocks)
        int* radixCounts;
        int* radixOffsets;

        int numThreadBlocks;
        int threadsPerBlock;
    };

    class RadixSort {
    private:
        RadixSortData m_radixData;
        bool m_radixInitialized;
        sycl::queue& m_queue;  // Reference, not copy!
        int m_maxBodies;

    public:
        // Constructor takes queue reference
        RadixSort(sycl::queue& queue);
        ~RadixSort();

        void init(int maxBodies);
        void cleanup();

        // Sort cellHash and bodyIndex arrays in-place
        void sort(int* cellHash, int* bodyIndex, int numBodies);
        void radixSortPass(int bitShift, int numBodies);

        // Getter for checking if initialized
        bool isInitialized() const { return m_radixInitialized; }
    };

}

#endif // _GPU_RADIX_SORT_H_