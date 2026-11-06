#include "mesh_primitives.hpp"
#include "mesh_renderable.hpp"

#include <cmath>
#include <initializer_list>

std::shared_ptr<MeshRenderable> createGeomMesh(GeomMesh geomMesh)
{
    std::shared_ptr<MeshGeometry> geometry;

    switch (geomMesh) {
    case GeomMesh::Cube:
        geometry = createCubeMeshGeometry();
        break;

    case GeomMesh::Sphere:
        geometry = createSphereMeshGeometry();
        break;

    case GeomMesh::Plane:
        geometry = createPlaneMeshGeometry();
        break;

    case GeomMesh::Torus:
        geometry = createTorusMeshGeometry();
        break;
    }

    return std::make_shared<MeshRenderable>(geometry);
}

std::shared_ptr<MeshGeometry> createCubeMeshGeometry(float size)
{
    const float halfSize = size * 0.5f;

    std::shared_ptr<MeshGeometry> mesh = std::make_shared<MeshGeometry>();

    mesh->m_positions = {
        glm::vec3(-halfSize, -halfSize, -halfSize),
        glm::vec3( halfSize, -halfSize, -halfSize),
        glm::vec3( halfSize,  halfSize, -halfSize),
        glm::vec3(-halfSize,  halfSize, -halfSize),
        glm::vec3(-halfSize, -halfSize,  halfSize),
        glm::vec3( halfSize, -halfSize,  halfSize),
        glm::vec3( halfSize,  halfSize,  halfSize),
        glm::vec3(-halfSize,  halfSize,  halfSize)
    };

    mesh->m_faceOffsets = {0, 4, 8, 12, 16, 20, 24};
    mesh->m_cornerVertices = {
        0, 3, 2, 1, // Back   (-Z)
        4, 5, 6, 7, // Front  (+Z)
        0, 4, 7, 3, // Left   (-X)
        1, 2, 6, 5, // Right  (+X)
        0, 1, 5, 4, // Bottom (-Y)
        3, 7, 6, 2  // Top    (+Y)
    };

    mesh->m_cornerUVs = {
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)
    };

    mesh->m_cornerNormals = {
        glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3( 0.0f,  0.0f, -1.0f),
        glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3( 0.0f,  0.0f,  1.0f),
        glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(-1.0f,  0.0f,  0.0f),
        glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3( 1.0f,  0.0f,  0.0f),
        glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3( 0.0f, -1.0f,  0.0f),
        glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3( 0.0f,  1.0f,  0.0f)
    };

    mesh->m_faceMaterials = {0, 0, 0, 0, 0, 0};

    return mesh;
}

std::shared_ptr<MeshGeometry> createPlaneMeshGeometry(float width, float depth)
{
    const float halfWidth = width * 0.5f;
    const float halfDepth = depth * 0.5f;

    std::shared_ptr<MeshGeometry> mesh = std::make_shared<MeshGeometry>();

    mesh->m_positions = {
        glm::vec3(-halfWidth, 0.0f,  halfDepth),
        glm::vec3( halfWidth, 0.0f,  halfDepth),
        glm::vec3( halfWidth, 0.0f, -halfDepth),
        glm::vec3(-halfWidth, 0.0f, -halfDepth)
    };

    mesh->m_faceOffsets = {0, 4};
    mesh->m_cornerVertices = {0, 1, 2, 3};
    mesh->m_cornerUVs = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f)
    };
    mesh->m_cornerNormals = {
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    };
    mesh->m_faceMaterials = {0};

    return mesh;
}

std::shared_ptr<MeshGeometry> createSphereMeshGeometry(float radius,
                                                        int sectorCount,
                                                        int stackCount)
{
    constexpr float pi = 3.14159265358979323846f;

    std::shared_ptr<MeshGeometry> mesh = std::make_shared<MeshGeometry>();
    if (sectorCount < 3 || stackCount < 2) {
        return mesh;
    }

    mesh->m_positions.push_back(glm::vec3(0.0f, radius, 0.0f));

    for (int stack = 1; stack < stackCount; ++stack) {
        const float stackAngle = pi * 0.5f - pi * static_cast<float>(stack) / stackCount;
        const float ringRadius = radius * std::cos(stackAngle);
        const float y = radius * std::sin(stackAngle);

        for (int sector = 0; sector < sectorCount; ++sector) {
            const float sectorAngle = 2.0f * pi * static_cast<float>(sector) / sectorCount;
            mesh->m_positions.emplace_back(ringRadius * std::cos(sectorAngle),
                                           y,
                                           ringRadius * std::sin(sectorAngle));
        }
    }

    const uint32_t bottomVertex = static_cast<uint32_t>(mesh->m_positions.size());
    mesh->m_positions.push_back(glm::vec3(0.0f, -radius, 0.0f));

    const auto addCorner = [&](uint32_t positionIndex, float u, float v) {
        mesh->m_cornerVertices.push_back(positionIndex);
        mesh->m_cornerUVs.emplace_back(u, v);
        mesh->m_cornerNormals.push_back(glm::normalize(mesh->m_positions[positionIndex]));
    };

    const auto addFace = [&](const std::initializer_list<uint32_t>& positions,
                             const std::initializer_list<glm::vec2>& uvs) {
        mesh->m_faceOffsets.push_back(static_cast<uint32_t>(mesh->m_cornerVertices.size()));

        auto uv = uvs.begin();
        for (const uint32_t positionIndex : positions) {
            addCorner(positionIndex, uv->x, uv->y);
            ++uv;
        }

        mesh->m_faceMaterials.push_back(0);
    };

    for (int sector = 0; sector < sectorCount; ++sector) {
        const int nextSector = (sector + 1) % sectorCount;
        const float u0 = static_cast<float>(sector) / sectorCount;
        const float u1 = static_cast<float>(sector + 1) / sectorCount;

        addFace({0,
                 static_cast<uint32_t>(1 + nextSector),
                 static_cast<uint32_t>(1 + sector)},
                {glm::vec2((u0 + u1) * 0.5f, 0.0f),
                 glm::vec2(u1, 1.0f / stackCount),
                 glm::vec2(u0, 1.0f / stackCount)});
    }

    for (int stack = 1; stack < stackCount - 1; ++stack) {
        const uint32_t upperRing = 1 + static_cast<uint32_t>(stack - 1) * sectorCount;
        const uint32_t lowerRing = upperRing + sectorCount;
        const float v0 = static_cast<float>(stack) / stackCount;
        const float v1 = static_cast<float>(stack + 1) / stackCount;

        for (int sector = 0; sector < sectorCount; ++sector) {
            const int nextSector = (sector + 1) % sectorCount;
            const float u0 = static_cast<float>(sector) / sectorCount;
            const float u1 = static_cast<float>(sector + 1) / sectorCount;

            addFace({upperRing + static_cast<uint32_t>(sector),
                     upperRing + static_cast<uint32_t>(nextSector),
                     lowerRing + static_cast<uint32_t>(nextSector),
                     lowerRing + static_cast<uint32_t>(sector)},
                    {glm::vec2(u0, v0),
                     glm::vec2(u1, v0),
                     glm::vec2(u1, v1),
                     glm::vec2(u0, v1)});
        }
    }

    const uint32_t lastRing = 1 + static_cast<uint32_t>(stackCount - 2) * sectorCount;
    for (int sector = 0; sector < sectorCount; ++sector) {
        const int nextSector = (sector + 1) % sectorCount;
        const float u0 = static_cast<float>(sector) / sectorCount;
        const float u1 = static_cast<float>(sector + 1) / sectorCount;

        addFace({lastRing + static_cast<uint32_t>(sector),
                 lastRing + static_cast<uint32_t>(nextSector),
                 bottomVertex},
                {glm::vec2(u0, static_cast<float>(stackCount - 1) / stackCount),
                 glm::vec2(u1, static_cast<float>(stackCount - 1) / stackCount),
                 glm::vec2((u0 + u1) * 0.5f, 1.0f)});
    }

    mesh->m_faceOffsets.push_back(static_cast<uint32_t>(mesh->m_cornerVertices.size()));

    return mesh;
}

std::shared_ptr<MeshGeometry> createTorusMeshGeometry(float majorRadius,
                                                       float minorRadius,
                                                       int majorSegments,
                                                       int minorSegments)
{
    constexpr float pi = 3.14159265358979323846f;

    std::shared_ptr<MeshGeometry> mesh = std::make_shared<MeshGeometry>();
    if (majorSegments < 3 || minorSegments < 3) {
        return mesh;
    }

    for (int major = 0; major < majorSegments; ++major) {
        const float u = 2.0f * pi * static_cast<float>(major) / majorSegments;
        const float cosU = std::cos(u);
        const float sinU = std::sin(u);

        for (int minor = 0; minor < minorSegments; ++minor) {
            const float v = 2.0f * pi * static_cast<float>(minor) / minorSegments;
            const float ringRadius = majorRadius + minorRadius * std::cos(v);

            mesh->m_positions.emplace_back(ringRadius * cosU,
                                           ringRadius * sinU,
                                           minorRadius * std::sin(v));
        }
    }

    mesh->m_faceOffsets.push_back(0);

    for (int major = 0; major < majorSegments; ++major) {
        const int nextMajor = (major + 1) % majorSegments;
        const float u0 = static_cast<float>(major) / majorSegments;
        const float u1 = static_cast<float>(major + 1) / majorSegments;

        for (int minor = 0; minor < minorSegments; ++minor) {
            const int nextMinor = (minor + 1) % minorSegments;
            const float v0 = static_cast<float>(minor) / minorSegments;
            const float v1 = static_cast<float>(minor + 1) / minorSegments;

            const uint32_t a = static_cast<uint32_t>(major * minorSegments + minor);
            const uint32_t b = static_cast<uint32_t>(nextMajor * minorSegments + minor);
            const uint32_t c = static_cast<uint32_t>(major * minorSegments + nextMinor);
            const uint32_t d = static_cast<uint32_t>(nextMajor * minorSegments + nextMinor);

            mesh->m_cornerVertices.insert(mesh->m_cornerVertices.end(), {a, b, d, c});
            mesh->m_cornerUVs.insert(mesh->m_cornerUVs.end(), {
                glm::vec2(u0, v0),
                glm::vec2(u1, v0),
                glm::vec2(u1, v1),
                glm::vec2(u0, v1)
            });

            const auto addNormal = [&](float u, float v) {
                mesh->m_cornerNormals.emplace_back(std::cos(v) * std::cos(u),
                                                    std::cos(v) * std::sin(u),
                                                    std::sin(v));
            };

            addNormal(2.0f * pi * u0, 2.0f * pi * v0);
            addNormal(2.0f * pi * u1, 2.0f * pi * v0);
            addNormal(2.0f * pi * u1, 2.0f * pi * v1);
            addNormal(2.0f * pi * u0, 2.0f * pi * v1);

            mesh->m_faceMaterials.push_back(0);
            mesh->m_faceOffsets.push_back(static_cast<uint32_t>(mesh->m_cornerVertices.size()));
        }
    }

    return mesh;
}
