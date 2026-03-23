#if !defined(_GPU_SPATIAL_GRID_HPP_)
#define _GPU_SPATIAL_GRID_HPP_
#include "gpu_includes.hpp"
#include "gpu_uniform_grid.hpp"
#include "gpu_radix_sort.hpp"
#include <memory>

namespace gpu
{

    class SpatialGrid{

        sycl::queue& m_queue;
        UniformGridData m_gridData;
        std::unique_ptr<RadixSort> m_radixSort;
        bool m_initialized;
        float m_worldOffset;


    public:
        SpatialGrid(sycl::queue& queue);
        ~SpatialGrid();

        void init(int maxBodies, float cellSize, float worldSize);
        void cleanup();

        void computeCellHashes(float* x_pos, float* y_pos, float* z_pos,
            int* bodyMode, int numBodies);
        void sort(int numBodies);
        void findCellBoundaries(int numBodies);

        const UniformGridData& getGridData() const { return m_gridData; }
        bool isInitialized() const { return m_initialized; }

    };
    
} // namespace gpu





#endif // _GPU_SPATIAL_GRID_HPP_
