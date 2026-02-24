#if !defined(_UNIFORM_GRID_H_)
#define _UNIFORM_GRID_H_

#include <vector>
#include <set>
#include <memory>
#include "Physics/RigidBody/rigid_body.hpp"
#include "Vector/vector3.hpp"
#include "ComputationalGeom/aabb.hpp"

class UniformGrid {
private:
    fungt::Vec3 m_worldMin;
    fungt::Vec3 m_worldMax;
    float m_cellSize;
    int m_gridSizeX;
    int m_gridSizeY;
    int m_gridSizeZ;

    // Each cell contains indices of bodies that overlap it
    std::vector<std::vector<int>> m_cells;

public:
    UniformGrid(const fungt::Vec3& worldMin, const fungt::Vec3& worldMax, float cellSize)
        : m_worldMin(worldMin)
        , m_worldMax(worldMax)
        , m_cellSize(cellSize)
    {
        // Calculate grid dimensions
        fungt::Vec3 worldSize = worldMax - worldMin;
        m_gridSizeX = static_cast<int>(std::ceil(worldSize.x / cellSize));
        m_gridSizeY = static_cast<int>(std::ceil(worldSize.y / cellSize));
        m_gridSizeZ = static_cast<int>(std::ceil(worldSize.z / cellSize));

        // Allocate cells
        int totalCells = m_gridSizeX * m_gridSizeY * m_gridSizeZ;
        m_cells.resize(totalCells);
    }

    void clear() {
        for (auto& cell : m_cells) {
            cell.clear();
        }
    }

    std::vector<std::pair<int, int>> getPotentialPairs(
        const std::vector<std::shared_ptr<RigidBody>>& bodies)
    {
        clear();

        // Step 1: Insert all bodies into grid cells
        for (int i = 0; i < bodies.size(); ++i) {
            if (!bodies[i]) continue;
            insertBody(i, bodies[i]);
        }

        // Step 2: Find potential collision pairs
        std::set<std::pair<int, int>> uniquePairs;

        for (int i = 0; i < bodies.size(); ++i) {
            if (!bodies[i]) continue;

            AABB bounds = bodies[i]->m_shape->getBoundingBox(bodies[i]->m_pos);

            // Get cell range this body overlaps
            int minX, minY, minZ, maxX, maxY, maxZ;
            getCellRange(bounds, minX, minY, minZ, maxX, maxY, maxZ);

            // Check all cells this body overlaps
            for (int x = minX; x <= maxX; ++x) {
                for (int y = minY; y <= maxY; ++y) {
                    for (int z = minZ; z <= maxZ; ++z) {
                        int cellIdx = getCellIndex(x, y, z);
                        if (cellIdx < 0 || cellIdx >= m_cells.size()) continue;

                        // Check against all bodies in this cell
                        for (int j : m_cells[cellIdx]) {
                            if (i < j) {  // Avoid duplicates and self-collision
                                uniquePairs.insert({ i, j });
                            }
                        }
                    }
                }
            }
        }

        // Convert set to vector
        std::vector<std::pair<int, int>> pairs;
        pairs.assign(uniquePairs.begin(), uniquePairs.end());
        return pairs;
    }

private:
    void insertBody(int bodyIndex, const std::shared_ptr<RigidBody>& body) {
        AABB bounds = body->m_shape->getBoundingBox(body->m_pos);

        int minX, minY, minZ, maxX, maxY, maxZ;
        getCellRange(bounds, minX, minY, minZ, maxX, maxY, maxZ);

        // Insert into all overlapping cells
        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                for (int z = minZ; z <= maxZ; ++z) {
                    int cellIdx = getCellIndex(x, y, z);
                    if (cellIdx >= 0 && cellIdx < m_cells.size()) {
                        m_cells[cellIdx].push_back(bodyIndex);
                    }
                }
            }
        }
    }

    void getCellRange(const AABB& bounds, int& minX, int& minY, int& minZ,
        int& maxX, int& maxY, int& maxZ) const {
        fungt::Vec3 localMin = bounds.m_min - m_worldMin;
        fungt::Vec3 localMax = bounds.m_max - m_worldMin;

        minX = std::max(0, static_cast<int>(std::floor(localMin.x / m_cellSize)));
        minY = std::max(0, static_cast<int>(std::floor(localMin.y / m_cellSize)));
        minZ = std::max(0, static_cast<int>(std::floor(localMin.z / m_cellSize)));

        maxX = std::min(m_gridSizeX - 1, static_cast<int>(std::floor(localMax.x / m_cellSize)));
        maxY = std::min(m_gridSizeY - 1, static_cast<int>(std::floor(localMax.y / m_cellSize)));
        maxZ = std::min(m_gridSizeZ - 1, static_cast<int>(std::floor(localMax.z / m_cellSize)));
    }

    int getCellIndex(int x, int y, int z) const {
        if (x < 0 || x >= m_gridSizeX ||
            y < 0 || y >= m_gridSizeY ||
            z < 0 || z >= m_gridSizeZ) {
            return -1;
        }
        return x + y * m_gridSizeX + z * m_gridSizeX * m_gridSizeY;
    }
};

#endif // _UNIFORM_GRID_H_