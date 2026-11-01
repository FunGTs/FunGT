#if !defined(_MESHING_MESH_GEOMETRY_HPP_)
#define _MESHING_MESH_GEOMETRY_HPP_

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "include/glmath.hpp"

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
        if (!isValidFaceOffsets(m_faceOffsets, m_cornerVertices.size())) {
            return false;
        }

        for (const uint32_t vertexIndex : m_cornerVertices) {
            if (vertexIndex >= m_positions.size()) {
                return false;
            }
        }

        return (m_cornerUVs.empty() || m_cornerUVs.size() == m_cornerVertices.size()) &&
               (m_cornerNormals.empty() || m_cornerNormals.size() == m_cornerVertices.size()) &&
               (m_faceMaterials.empty() || m_faceMaterials.size() == faceCount());
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
