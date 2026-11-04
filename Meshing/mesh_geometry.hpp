#if !defined(_MESHING_MESH_GEOMETRY_HPP_)
#define _MESHING_MESH_GEOMETRY_HPP_

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "include/glmath.hpp"
#include "Material/material.hpp"

class MeshGeomGPU;
class Shader;

class MeshGeometry {
public:
    struct FaceCorners {
        const uint32_t* vertices = nullptr;
        size_t count = 0;
    };
    std::vector<glm::vec3> m_positions;
    std::vector<uint32_t> m_faceOffsets;
    std::vector<uint32_t> m_cornerVertices;
    std::vector<glm::vec2> m_cornerUVs;
    std::vector<glm::vec3> m_cornerNormals;
    std::vector<uint32_t> m_faceMaterials;
    std::vector<Material> m_materials = {Material::createDefaultMaterial()};
    MeshGeomGPU* m_gpuCache = nullptr;

    static void (*s_gpuBuild)(MeshGeometry&, MeshGeomGPU*&);
    static void (*s_gpuFree)(MeshGeomGPU*);
    static void (*s_gpuDraw)(MeshGeomGPU&, const MeshGeometry&, Shader&);

    ~MeshGeometry()
    {
        if (s_gpuFree && m_gpuCache) {
            s_gpuFree(m_gpuCache);
        }
    }

    void InitGraphics()
    {
        if (s_gpuBuild && !m_gpuCache) {
            s_gpuBuild(*this, m_gpuCache);
        }
    }

    void attachGPUCache(MeshGeomGPU* cache)
    {
        m_gpuCache = cache;
    }

    void draw(Shader& shader)
    {
        if (s_gpuDraw && m_gpuCache) {
            s_gpuDraw(*m_gpuCache, *this, shader);
        }
    }

    void clear()
    {
        m_positions.clear();
        m_faceOffsets.clear();
        m_cornerVertices.clear();
        m_cornerUVs.clear();
        m_cornerNormals.clear();
        m_faceMaterials.clear();
    }

    void setPositions(std::vector<glm::vec3> positions)
    {
        m_positions = std::move(positions);
    }

    bool setTopology(std::vector<uint32_t> faceOffsets,
                     std::vector<uint32_t> cornerVertices)
    {
        if (!isValidFaceOffsets(faceOffsets, cornerVertices.size())) {
            return false;
        }

        m_faceOffsets = std::move(faceOffsets);
        m_cornerVertices = std::move(cornerVertices);
        return true;
    }

    bool setCornerUVs(std::vector<glm::vec2> cornerUVs)
    {
        if (cornerUVs.size() != m_cornerVertices.size()) {
            return false;
        }

        m_cornerUVs = std::move(cornerUVs);
        return true;
    }

    bool setCornerNormals(std::vector<glm::vec3> cornerNormals)
    {
        if (cornerNormals.size() != m_cornerVertices.size()) {
            return false;
        }

        m_cornerNormals = std::move(cornerNormals);
        return true;
    }

    bool setFaceMaterials(std::vector<uint32_t> faceMaterials)
    {
        if (faceMaterials.size() != faceCount()) {
            return false;
        }

        m_faceMaterials = std::move(faceMaterials);
        return true;
    }

    std::vector<Material>& getMaterials()
    {
        return m_materials;
    }

    const std::vector<Material>& getMaterials() const
    {
        return m_materials;
    }

    void setMaterials(std::vector<Material> materials)
    {
        if (materials.empty()) {
            materials.push_back(Material::createDefaultMaterial());
        }

        m_materials = std::move(materials);
    }

    size_t vertexCount() const { return m_positions.size(); }
    size_t faceCount() const { return m_faceOffsets.empty() ? 0 : m_faceOffsets.size() - 1; }
    size_t cornerCount() const { return m_cornerVertices.size(); }

    FaceCorners faceCorners(size_t faceIndex) const
    {
        if (faceIndex >= faceCount()) {
            return {};
        }

        const size_t begin = m_faceOffsets[faceIndex];
        const size_t end = m_faceOffsets[faceIndex + 1];
        return {m_cornerVertices.data() + begin, end - begin};
    }

    const std::vector<glm::vec3>& positions() const { return m_positions; }
    const std::vector<uint32_t>& faceOffsets() const { return m_faceOffsets; }
    const std::vector<uint32_t>& cornerVertices() const { return m_cornerVertices; }
    const std::vector<glm::vec2>& cornerUVs() const { return m_cornerUVs; }
    const std::vector<glm::vec3>& cornerNormals() const { return m_cornerNormals; }
    const std::vector<uint32_t>& faceMaterials() const { return m_faceMaterials; }

    bool isValid() const
    {
        if (m_materials.empty() ||
            !isValidFaceOffsets(m_faceOffsets, m_cornerVertices.size())) {
            return false;
        }

        for (const uint32_t vertexIndex : m_cornerVertices) {
            if (vertexIndex >= m_positions.size()) {
                return false;
            }
        }

        if (!m_faceMaterials.empty()) {
            if (m_faceMaterials.size() != faceCount()) {
                return false;
            }

            for (const uint32_t materialIndex : m_faceMaterials) {
                if (materialIndex >= m_materials.size()) {
                    return false;
                }
            }
        }

        return (m_cornerUVs.empty() || m_cornerUVs.size() == m_cornerVertices.size()) &&
               (m_cornerNormals.empty() || m_cornerNormals.size() == m_cornerVertices.size());
    }

private:
    static bool isValidFaceOffsets(const std::vector<uint32_t>& faceOffsets,
                                   size_t cornerCount)
    {
        if (faceOffsets.empty()) {
            return cornerCount == 0;
        }

        if (faceOffsets.front() != 0 || faceOffsets.back() != cornerCount) {
            return false;
        }

        for (size_t i = 1; i < faceOffsets.size(); ++i) {
            if (faceOffsets[i] < faceOffsets[i - 1]) {
                return false;
            }
        }

        return true;
    }


};

#endif // _MESHING_MESH_GEOMETRY_HPP_
