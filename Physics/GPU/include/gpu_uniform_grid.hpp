#if !defined(_GPU_UNIFORM_GRID_H_)
#define _GPU_UNIFORM_GRID_H_

struct UniformGridData {
    // Grid parameters
    float cellSize;
    float invCellSize;
    int gridDimX, gridDimY, gridDimZ;
    int totalCells;

    // Per-body arrays
    int* cellHash;
    int* bodyIndex;

    // Per-cell arrays
    int* cellStart;
    int* cellEnd;
};

#endif // _GPU_UNIFORM_GRID_H_
