#include "triangle_extractor.hpp"
#include <iostream>

// Extract triangles from SimpleModel
std::vector<Triangle> extractTriangles(
    const SimpleModel& simpleModel,
    IDeviceTexture* textureManager)
{
    std::vector<Triangle> triangles;

    Model& model = simpleModel.getModel();
    const std::vector<std::unique_ptr<Mesh>>& meshes = model.getMeshes();

    // Get transformation matrix from SimpleModel
    glm::mat4 modelMatrix = simpleModel.getModelMatrix();
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

    for (auto& meshPtr : meshes) {
        const auto& vertices = meshPtr->m_vertex;
        const auto& indices = meshPtr->m_index;
        const auto& materials = meshPtr->m_material;
        const auto& textures = meshPtr->m_texture;

        // Setup Material ONCE per mesh
        MaterialData global_material;
        global_material.baseColor[0] = 0.922;
        global_material.baseColor[1] = 0.467;
        global_material.baseColor[2] = 0.882f;
        global_material.metallic = 0.0f;
        global_material.roughness = 0.5f;
        global_material.reflectance = 0.05f;
        global_material.emission = materials[0].m_emission;
        global_material.baseColorTexIdx = -1;

        // Load texture from mesh data
        if (!textures.empty() && textureManager != nullptr) {
            std::string texPath = textures[0].getPath();
            global_material.baseColorTexIdx = textureManager->loadTexture(texPath);
        }

        // Load material properties from mesh
        if (!materials.empty()) {
            global_material.baseColor[0] = materials[0].m_diffLigth.x;
            global_material.baseColor[1] = materials[0].m_diffLigth.y;
            global_material.baseColor[2] = materials[0].m_diffLigth.z;

            float Ns = materials[0].m_shininess;
            global_material.roughness = sqrtf(2.0f / (Ns + 2.0f));

            float avgSpec = (materials[0].m_specLight.x +
                materials[0].m_specLight.y +
                materials[0].m_specLight.z) / 3.0f;
            global_material.metallic = (avgSpec < 0.9f) ? 0.0f : 0.3f;
        }

        // Extract triangles from mesh
        for (size_t i = 0; i < indices.size(); i += 3) {
            const funGTVERTEX& v0 = vertices[indices[i + 0]];
            const funGTVERTEX& v1 = vertices[indices[i + 1]];
            const funGTVERTEX& v2 = vertices[indices[i + 2]];

            Triangle tri;

            // Transform positions
            glm::vec4 p0 = modelMatrix * glm::vec4(v0.position, 1.0f);
            glm::vec4 p1 = modelMatrix * glm::vec4(v1.position, 1.0f);
            glm::vec4 p2 = modelMatrix * glm::vec4(v2.position, 1.0f);

            tri.v0 = fungt::Vec3(p0.x, p0.y, p0.z);
            tri.v1 = fungt::Vec3(p1.x, p1.y, p1.z);
            tri.v2 = fungt::Vec3(p2.x, p2.y, p2.z);

            // Transform normals
            glm::vec3 n0 = normalMatrix * v0.normal;
            glm::vec3 n1 = normalMatrix * v1.normal;
            glm::vec3 n2 = normalMatrix * v2.normal;

            tri.n0 = fungt::toFungtVec3(glm::normalize(n0));
            tri.n1 = fungt::toFungtVec3(glm::normalize(n1));
            tri.n2 = fungt::toFungtVec3(glm::normalize(n2));

            // UVs
            tri.uvs[0][0] = v0.texcoord.x;
            tri.uvs[0][1] = v0.texcoord.y;
            tri.uvs[1][0] = v1.texcoord.x;
            tri.uvs[1][1] = v1.texcoord.y;
            tri.uvs[2][0] = v2.texcoord.x;
            tri.uvs[2][1] = v2.texcoord.y;

            // Assign material
            tri.material = global_material;

            triangles.push_back(std::move(tri));
        }
    }

    return triangles;
}

// Extract triangles from SimpleGeometry
std::vector<Triangle> extractTriangles(
    const SimpleGeometry& geometry,
    IDeviceTexture* textureManager)
{
    std::vector<Triangle> triangles;

    auto primitive = geometry.getPrimitive();
    if (!primitive) {
        std::cerr << "ERROR: Geometry has no primitive!" << std::endl;
        return triangles;
    }

    const Primitive* constPrimitive = primitive.get();
    const std::vector<PrimitiveVertex>& vertices = constPrimitive->getVertices();
    const std::vector<unsigned int>& indices = constPrimitive->getIndices();

    glm::mat4 modelMatrix = geometry.getModelMatrix();
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

    const auto& material = geometry.getMaterial();

    // Load texture if exists
    int baseColorTexId = -1;
    if (geometry.isTexturized() && textureManager != nullptr) {
        std::string texPath = constPrimitive->texture.getPath();
        baseColorTexId = textureManager->loadTexture(texPath);
    }

    // Extract triangles
    for (size_t i = 0; i < indices.size(); i += 3) {
        const PrimitiveVertex& v0 = vertices[indices[i]];
        const PrimitiveVertex& v1 = vertices[indices[i + 1]];
        const PrimitiveVertex& v2 = vertices[indices[i + 2]];

        Triangle tri;

        // Transform positions
        glm::vec4 p0 = modelMatrix * glm::vec4(v0.position, 1.0f);
        glm::vec4 p1 = modelMatrix * glm::vec4(v1.position, 1.0f);
        glm::vec4 p2 = modelMatrix * glm::vec4(v2.position, 1.0f);

        tri.v0 = fungt::Vec3(p0.x, p0.y, p0.z);
        tri.v1 = fungt::Vec3(p1.x, p1.y, p1.z);
        tri.v2 = fungt::Vec3(p2.x, p2.y, p2.z);

        // Transform normals
        glm::vec3 n0 = glm::normalize(normalMatrix * v0.normal);
        glm::vec3 n1 = glm::normalize(normalMatrix * v1.normal);
        glm::vec3 n2 = glm::normalize(normalMatrix * v2.normal);

        tri.n0 = fungt::Vec3(n0.x, n0.y, n0.z);
        tri.n1 = fungt::Vec3(n1.x, n1.y, n1.z);
        tri.n2 = fungt::Vec3(n2.x, n2.y, n2.z);

        // UVs
        tri.uvs[0][0] = v0.texcoord.x;
        tri.uvs[0][1] = v0.texcoord.y;
        tri.uvs[1][0] = v1.texcoord.x;
        tri.uvs[1][1] = v1.texcoord.y;
        tri.uvs[2][0] = v2.texcoord.x;
        tri.uvs[2][1] = v2.texcoord.y;

        // Material
        tri.material.baseColor[0] = material.baseColor.x;
        tri.material.baseColor[1] = material.baseColor.y;
        tri.material.baseColor[2] = material.baseColor.z;
        tri.material.roughness = material.roughness;
        tri.material.metallic = material.metallic;
        tri.material.baseColorTexIdx = baseColorTexId;

        triangles.push_back(tri);
    }

    return triangles;
}

// Extract triangles from ALL objects in SceneManager
std::vector<Triangle> extractTriangles(
    SceneManager* sceneManager,
    IDeviceTexture* textureManager)
{
    std::vector<Triangle> allTriangles;

    if (!sceneManager) {
        std::cerr << "ERROR: SceneManager is null!" << std::endl;
        return allTriangles;
    }

    const auto& renderables = sceneManager->getRenderable();

    std::cout << "Extracting triangles from " << renderables.size() << " objects..." << std::endl;

    for (const auto& obj : renderables) {
        // Try as SimpleModel
        auto model = std::dynamic_pointer_cast<SimpleModel>(obj);
        if (model) {
            auto tris = extractTriangles(*model, textureManager);
            allTriangles.insert(allTriangles.end(), tris.begin(), tris.end());
            std::cout << "  Extracted " << tris.size() << " triangles from SimpleModel" << std::endl;
            continue;
        }

        // Try as SimpleGeometry
        auto geom = std::dynamic_pointer_cast<SimpleGeometry>(obj);
        if (geom) {
            auto tris = extractTriangles(*geom, textureManager);
            allTriangles.insert(allTriangles.end(), tris.begin(), tris.end());
            std::cout << "  Extracted " << tris.size() << " triangles from SimpleGeometry" << std::endl;
        }
    }

    std::cout << "Total triangles extracted: " << allTriangles.size() << std::endl;

    return allTriangles;
}