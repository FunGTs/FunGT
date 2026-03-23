#include "gpu_spatial_grid.hpp"


gpu::SpatialGrid::SpatialGrid(sycl::queue& queue)
    : m_queue(queue), m_initialized(false), m_worldOffset(0.0f) {
}

gpu::SpatialGrid::~SpatialGrid() {
    cleanup();
}

void gpu::SpatialGrid::init(int maxBodies, float cellSize, float worldSize)
{

    m_gridData.cellSize = cellSize; 
    m_gridData.invCellSize = 1.0/cellSize;
    int gridDim = staic_cast<int> (worldSize/cellSize);

    m_gridData.gridDimX = gridDim;
    m_gridData.gridDimY = gridDim;
    m_gridData.gridDimZ = gridDim;

    m_gridData.totalCells = m_gridData.gridDimX * m_gridData.gridDimY * m_gridData.gridDimZ;

    m_worldOffset = worldSize / 2.0f;
    m_gridData.cellHash = sycl::malloc_device<int>(maxBodies, m_queue);
    m_gridData.bodyIndex = sycl::malloc_device<int>(maxBodies, m_queue);
    m_gridData.cellStart = sycl::malloc_device<int>(m_gridData.totalCells, m_queue);
    m_gridData.cellEnd = sycl::malloc_device<int>(m_gridData.totalCells, m_queue);


    m_radixSort = std::make_unique<RadixSort>(m_queue);
    m_radixSort->init(maxBodies);

    m_initialized = true;

    std::cout << "SpatialGrid initialized: "
        << m_gridData.gridDimX << "x"
        << m_gridData.gridDimY << "x"
        << m_gridData.gridDimZ
        << " = " << m_gridData.totalCells << " cells"
        << " (cellSize=" << cellSize << ", offset=" << m_worldOffset << ")\n";



}

void gpu::SpatialGrid::cleanup() {
    if (m_initialized) {
        sycl::free(m_gridData.cellHash, m_queue);
        sycl::free(m_gridData.bodyIndex, m_queue);
        sycl::free(m_gridData.cellStart, m_queue);
        sycl::free(m_gridData.cellEnd, m_queue);
        m_initialized = false;
    }
}

void gpu::SpatialGrid::computeCellHashes(float* x_pos, float* y_pos, float* z_pos,
    int*  bodyMode, int numBodies) {
    UniformGridData grid = m_gridData;
    int n = numBodies;
    float offset = m_worldOffset;

    std::size_t xdim = 32;
    std::size_t ydim = (n + xdim - 1) / xdim;

    m_queue.submit([&](sycl::handler& cgh) {
        cgh.parallel_for(sycl::range<2>(ydim, xdim), [=](sycl::item<2> item) {
            std::size_t i = item[0] * xdim + item[1];
            if (i >= n) return;

            if (bodyMode != nullptr && bodyMode[i] == 0) {
                grid.cellHash[i] = -1;
                grid.bodyIndex[i] = i;
                return;
            }

            float px = x_pos[i];
            float py = y_pos[i];
            float pz = z_pos[i];

            float offsetX = px + offset;
            float offsetY = py + offset;
            float offsetZ = pz + offset;

            int cellX = static_cast<int>(sycl::floor(offsetX * grid.invCellSize));
            int cellY = static_cast<int>(sycl::floor(offsetY * grid.invCellSize));
            int cellZ = static_cast<int>(sycl::floor(offsetZ * grid.invCellSize));

            cellX = sycl::clamp(cellX, 0, grid.gridDimX - 1);
            cellY = sycl::clamp(cellY, 0, grid.gridDimY - 1);
            cellZ = sycl::clamp(cellZ, 0, grid.gridDimZ - 1);

            int cellHash = cellX + cellY * grid.gridDimX + cellZ * grid.gridDimX * grid.gridDimY;

            grid.cellHash[i] = cellHash;
            grid.bodyIndex[i] = i;
            });
        }).wait();
}

void gpu::SpatialGrid::sort(int numBodies) {
    m_radixSort->sort(m_gridData.cellHash, m_gridData.bodyIndex, numBodies);
}

void gpu::SpatialGrid::findCellBoundaries(int numBodies) {
    UniformGridData grid = m_gridData;
    int n = numBodies;

    m_queue.fill(grid.cellStart, -1, grid.totalCells).wait();
    m_queue.fill(grid.cellEnd, -1, grid.totalCells).wait();

    std::size_t xdim = 32;
    std::size_t ydim = (n + xdim - 1) / xdim;

    m_queue.submit([&](sycl::handler& cgh) {
        cgh.parallel_for(sycl::range<2>(ydim, xdim), [=](sycl::item<2> item) {
            std::size_t i = item[0] * xdim + item[1];
            if (i >= n) return;

            int cellHash = grid.cellHash[i];
            if (cellHash < 0) return;

            if (i == 0 || grid.cellHash[i - 1] != cellHash) {
                grid.cellStart[cellHash] = i;
            }

            if (i == n - 1 || grid.cellHash[i + 1] != cellHash) {
                grid.cellEnd[cellHash] = i + 1;
            }
            });
        }).wait();
}