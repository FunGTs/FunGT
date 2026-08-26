#include "marching_cubes.hpp"
#include "mc_tables.hpp"
#include <cmath>

namespace fungt {

    static fungt::Vec3 interpolateVertex(
        float isovalue,
        fungt::Vec3 p0, float v0,
        fungt::Vec3 p1, float v1)
    {
        if (std::abs(v1 - v0) < 1e-6f)
            return p0;
        float t = (isovalue - v0) / (v1 - v0);
        return fungt::Vec3(
            p0.x + t * (p1.x - p0.x),
            p0.y + t * (p1.y - p0.y),
            p0.z + t * (p1.z - p0.z)
        );
    }

    static fungt::Vec3 computeNormal(
        const std::function<float(fungt::Vec3)>& field,
        fungt::Vec3 p,
        float h)
    {
        float dx = field(fungt::Vec3(p.x + h, p.y, p.z))
            - field(fungt::Vec3(p.x - h, p.y, p.z));
        float dy = field(fungt::Vec3(p.x, p.y + h, p.z))
            - field(fungt::Vec3(p.x, p.y - h, p.z));
        float dz = field(fungt::Vec3(p.x, p.y, p.z + h))
            - field(fungt::Vec3(p.x, p.y, p.z - h));
        fungt::Vec3 n(dx, dy, dz);
        return n.normalize();
    }

    MarchingCubes::MarchingCubes()
        : m_resolution{ 64, 64, 64 }
        , m_isovalue(0.0f)
        , m_boundsMin(-1.0f, -1.0f, -1.0f)
        , m_boundsMax(1.0f, 1.0f, 1.0f)
    {
    }


    MarchingCubes::~MarchingCubes()
    {
    }

    void MarchingCubes::setField(std::function<float(fungt::Vec3)> field)
    {
        m_field = field;
    }

    void MarchingCubes::sampleScalarFieldInside(fungt::Vec3 min, fungt::Vec3 max)
    {
        m_boundsMin = min;
        m_boundsMax = max;
    }

    void MarchingCubes::setResolution(int x, int y, int z)
    {
        m_resolution = { x, y, z };
    }

    void MarchingCubes::setIsovalue(float iso)
    {
        m_isovalue = iso;
    }

    const std::vector<PrimitiveVertex>& MarchingCubes::getVertices() const
    {
        return m_vertices;
    }

    const std::vector<unsigned int>& MarchingCubes::getIndices() const
    {
        return m_indices;
    }
    void MarchingCubes::execute()
    {
        m_vertices.clear();
        m_indices.clear();

        int nx = m_resolution[0];
        int ny = m_resolution[1];
        int nz = m_resolution[2];

        float stepX = (m_boundsMax.x - m_boundsMin.x) / (float)(nx - 1);
        float stepY = (m_boundsMax.y - m_boundsMin.y) / (float)(ny - 1);
        float stepZ = (m_boundsMax.z - m_boundsMin.z) / (float)(nz - 1);

        float h = std::min({ stepX, stepY, stepZ }) * 0.5f;

        // evaluate field at every grid point
        std::vector<float> grid(nx * ny * nz);
        for (int iz = 0; iz < nz; iz++) {
            for (int iy = 0; iy < ny; iy++) {
                for (int ix = 0; ix < nx; ix++) {
                    fungt::Vec3 p(
                        m_boundsMin.x + ix * stepX,
                        m_boundsMin.y + iy * stepY,
                        m_boundsMin.z + iz * stepZ
                    );
                    grid[ix + nx * (iy + ny * iz)] = m_field(p);
                }
            }
        }

        auto gridPos = [&](int ix, int iy, int iz) -> fungt::Vec3 {
            return fungt::Vec3(
                m_boundsMin.x + ix * stepX,
                m_boundsMin.y + iy * stepY,
                m_boundsMin.z + iz * stepZ
            );
            };

        auto gridVal = [&](int ix, int iy, int iz) -> float {
            return grid[ix + nx * (iy + ny * iz)];
            };

        unsigned int vertexCount = 0;

        // march over every cell
        for (int iz = 0; iz < nz - 1; iz++) {
            for (int iy = 0; iy < ny - 1; iy++) {
                for (int ix = 0; ix < nx - 1; ix++) {

                    // 8 corners of this cell
                    fungt::Vec3 corners[8] = {
                        gridPos(ix,     iy,     iz),
                        gridPos(ix + 1, iy,     iz),
                        gridPos(ix + 1, iy,     iz + 1),
                        gridPos(ix,     iy,     iz + 1),
                        gridPos(ix,     iy + 1, iz),
                        gridPos(ix + 1, iy + 1, iz),
                        gridPos(ix + 1, iy + 1, iz + 1),
                        gridPos(ix,     iy + 1, iz + 1)
                    };

                    float vals[8] = {
                        gridVal(ix,     iy,     iz),
                        gridVal(ix + 1, iy,     iz),
                        gridVal(ix + 1, iy,     iz + 1),
                        gridVal(ix,     iy,     iz + 1),
                        gridVal(ix,     iy + 1, iz),
                        gridVal(ix + 1, iy + 1, iz),
                        gridVal(ix + 1, iy + 1, iz + 1),
                        gridVal(ix,     iy + 1, iz + 1)
                    };

                    // build the 8-bit index into the LUT
                    int cubeIndex = 0;
                    for (int i = 0; i < 8; i++) {
                        if (vals[i] < m_isovalue)
                            cubeIndex |= (1 << i);
                    }

                    if (edgeTable[cubeIndex] == 0)
                        continue;

                    // interpolate vertices on each crossed edge
                    fungt::Vec3 edgeVerts[12];

                    if (edgeTable[cubeIndex] & 1)
                        edgeVerts[0] = interpolateVertex(m_isovalue, corners[0], vals[0], corners[1], vals[1]);
                    if (edgeTable[cubeIndex] & 2)
                        edgeVerts[1] = interpolateVertex(m_isovalue, corners[1], vals[1], corners[2], vals[2]);
                    if (edgeTable[cubeIndex] & 4)
                        edgeVerts[2] = interpolateVertex(m_isovalue, corners[2], vals[2], corners[3], vals[3]);
                    if (edgeTable[cubeIndex] & 8)
                        edgeVerts[3] = interpolateVertex(m_isovalue, corners[3], vals[3], corners[0], vals[0]);
                    if (edgeTable[cubeIndex] & 16)
                        edgeVerts[4] = interpolateVertex(m_isovalue, corners[4], vals[4], corners[5], vals[5]);
                    if (edgeTable[cubeIndex] & 32)
                        edgeVerts[5] = interpolateVertex(m_isovalue, corners[5], vals[5], corners[6], vals[6]);
                    if (edgeTable[cubeIndex] & 64)
                        edgeVerts[6] = interpolateVertex(m_isovalue, corners[6], vals[6], corners[7], vals[7]);
                    if (edgeTable[cubeIndex] & 128)
                        edgeVerts[7] = interpolateVertex(m_isovalue, corners[7], vals[7], corners[4], vals[4]);
                    if (edgeTable[cubeIndex] & 256)
                        edgeVerts[8] = interpolateVertex(m_isovalue, corners[0], vals[0], corners[4], vals[4]);
                    if (edgeTable[cubeIndex] & 512)
                        edgeVerts[9] = interpolateVertex(m_isovalue, corners[1], vals[1], corners[5], vals[5]);
                    if (edgeTable[cubeIndex] & 1024)
                        edgeVerts[10] = interpolateVertex(m_isovalue, corners[2], vals[2], corners[6], vals[6]);
                    if (edgeTable[cubeIndex] & 2048)
                        edgeVerts[11] = interpolateVertex(m_isovalue, corners[3], vals[3], corners[7], vals[7]);

                    // emit triangles
                    for (int i = 0; triTable[cubeIndex][i] != -1; i += 3) {
                        fungt::Vec3 v0 = edgeVerts[triTable[cubeIndex][i]];
                        fungt::Vec3 v1 = edgeVerts[triTable[cubeIndex][i + 1]];
                        fungt::Vec3 v2 = edgeVerts[triTable[cubeIndex][i + 2]];

                        fungt::Vec3 n0 = computeNormal(m_field, v0, h);
                        fungt::Vec3 n1 = computeNormal(m_field, v1, h);
                        fungt::Vec3 n2 = computeNormal(m_field, v2, h);

                        PrimitiveVertex pv0, pv1, pv2;

                        pv0.position = glm::vec3(v0.x, v0.y, v0.z);
                        pv0.normal = glm::vec3(n0.x, n0.y, n0.z);
                        pv0.texcoord = glm::vec2(0.f);

                        pv1.position = glm::vec3(v1.x, v1.y, v1.z);
                        pv1.normal = glm::vec3(n1.x, n1.y, n1.z);
                        pv1.texcoord = glm::vec2(0.f);

                        pv2.position = glm::vec3(v2.x, v2.y, v2.z);
                        pv2.normal = glm::vec3(n2.x, n2.y, n2.z);
                        pv2.texcoord = glm::vec2(0.f);

                        m_vertices.push_back(pv0);
                        m_vertices.push_back(pv1);
                        m_vertices.push_back(pv2);

                        m_indices.push_back(vertexCount++);
                        m_indices.push_back(vertexCount++);
                        m_indices.push_back(vertexCount++);
                    }
                }
            }
        }
    }

}