#include "mesh_importer.hpp"

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>

#include <cmath>
#include <filesystem>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

using TextureCache = std::unordered_map<std::string, std::shared_ptr<Texture>>;

struct NodeEntry {
    aiNode* m_node;
    aiMatrix4x4 m_parentTransform;
};

glm::mat4 toGlmMatrix(const aiMatrix4x4& matrix)
{
    return glm::mat4(
        matrix.a1, matrix.b1, matrix.c1, matrix.d1,
        matrix.a2, matrix.b2, matrix.c2, matrix.d2,
        matrix.a3, matrix.b3, matrix.c3, matrix.d3,
        matrix.a4, matrix.b4, matrix.c4, matrix.d4);
}

std::shared_ptr<Texture> loadTexture(const std::string& path, TextureCache& textureCache)
{
    const auto found = textureCache.find(path);
    if (found != textureCache.end()) {
        return found->second;
    }

    auto texture = std::make_shared<Texture>();
    texture->genTexture(path);
    texture->setPath(path);
    texture->setTypeName("texture_diffuse");

    textureCache[path] = texture;
    return texture;
}

std::shared_ptr<Texture> loadBaseColorTexture(aiMaterial* assimpMaterial,
                                              const std::filesystem::path& modelDirectory,
                                              TextureCache& textureCache)
{
    aiString texturePath;
    aiTextureType textureType = aiTextureType_DIFFUSE;

    if (assimpMaterial->GetTextureCount(textureType) == 0) {
#if ASSIMP_VERSION_MAJOR >= 5
        textureType = aiTextureType_BASE_COLOR;
#endif
    }

    if (assimpMaterial->GetTextureCount(textureType) == 0 ||
        assimpMaterial->GetTexture(textureType, 0, &texturePath) != AI_SUCCESS) {
        return nullptr;
    }

    const std::string relativePath = texturePath.C_Str();
    if (!relativePath.empty() && relativePath[0] == '*') {
        return nullptr;
    }

    const std::string fullPath = (modelDirectory / relativePath).lexically_normal().string();
    return loadTexture(fullPath, textureCache);
}

Material importMaterial(aiMaterial* assimpMaterial,
                        const std::filesystem::path& modelDirectory,
                        TextureCache& textureCache)
{
    aiString name;
    aiColor3D diffuseColor(0.8f, 0.8f, 0.8f);
    aiColor3D specularColor(0.04f, 0.04f, 0.04f);
    float shininess = 2.0f;

    assimpMaterial->Get(AI_MATKEY_NAME, name);
    assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
    assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
    assimpMaterial->Get(AI_MATKEY_SHININESS, shininess);

    const glm::vec3 baseColor(diffuseColor.r, diffuseColor.g, diffuseColor.b);
    const float roughness = glm::clamp(std::sqrt(2.0f / (shininess + 2.0f)), 0.05f, 1.0f);
    const float reflectance = glm::clamp(
        (specularColor.r + specularColor.g + specularColor.b) / 3.0f,
        0.0f,
        1.0f);

    Material material(baseColor, 0.0f, roughness, reflectance, name.C_Str());
    material.m_baseColorTexture = loadBaseColorTexture(
        assimpMaterial,
        modelDirectory,
        textureCache);
    return material;
}

std::shared_ptr<MeshGeometry> importMeshGeometry(aiMesh* assimpMesh,
                                                 const aiScene* scene,
                                                 const std::filesystem::path& modelDirectory,
                                                 TextureCache& textureCache)
{
    auto geometry = std::make_shared<MeshGeometry>();
    geometry->m_name = assimpMesh->mName.C_Str();
    geometry->m_positions.reserve(assimpMesh->mNumVertices);
    geometry->m_faceOffsets.push_back(0);

    for (unsigned int vertexIndex = 0; vertexIndex < assimpMesh->mNumVertices; ++vertexIndex) {
        const aiVector3D& position = assimpMesh->mVertices[vertexIndex];
        geometry->m_positions.emplace_back(position.x, position.y, position.z);
    }

    const bool hasVertexColors = assimpMesh->HasVertexColors(0);
    for (unsigned int faceIndex = 0; faceIndex < assimpMesh->mNumFaces; ++faceIndex) {
        const aiFace& face = assimpMesh->mFaces[faceIndex];

        for (unsigned int cornerIndex = 0; cornerIndex < face.mNumIndices; ++cornerIndex) {
            const uint32_t vertexIndex = face.mIndices[cornerIndex];
            geometry->m_cornerVertices.push_back(vertexIndex);

            if (assimpMesh->HasNormals()) {
                const aiVector3D& normal = assimpMesh->mNormals[vertexIndex];
                geometry->m_cornerNormals.emplace_back(normal.x, normal.y, normal.z);
            }
            else {
                geometry->m_cornerNormals.emplace_back(0.0f);
            }

            if (assimpMesh->HasTextureCoords(0)) {
                const aiVector3D& textureCoordinate = assimpMesh->mTextureCoords[0][vertexIndex];
                geometry->m_cornerUVs.emplace_back(textureCoordinate.x, textureCoordinate.y);
            }
            else {
                geometry->m_cornerUVs.emplace_back(0.0f);
            }

            if (hasVertexColors) {
                const aiColor4D& color = assimpMesh->mColors[0][vertexIndex];
                geometry->m_cornerColors.emplace_back(color.r, color.g, color.b, color.a);
            }
        }

        geometry->m_faceMaterials.push_back(0);
        geometry->m_faceOffsets.push_back(
            static_cast<uint32_t>(geometry->m_cornerVertices.size()));
    }

    Material material = Material::createDefaultMaterial();
    if (assimpMesh->mMaterialIndex < scene->mNumMaterials) {
        material = importMaterial(scene->mMaterials[assimpMesh->mMaterialIndex],
                                  modelDirectory,
                                  textureCache);
    }
    geometry->m_materials = {std::move(material)};

    return geometry;
}

}

std::vector<std::shared_ptr<MeshRenderable>> importStaticMesh(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::runtime_error(importer.GetErrorString());
    }

    const std::filesystem::path modelDirectory = std::filesystem::path(path).parent_path();
    TextureCache textureCache;
    std::vector<std::shared_ptr<MeshGeometry>> geometryCache(scene->mNumMeshes);
    std::vector<std::shared_ptr<MeshRenderable>> renderables;
    std::vector<NodeEntry> nodes;

    nodes.push_back({scene->mRootNode, aiMatrix4x4()});

    while (!nodes.empty()) {
        const NodeEntry entry = nodes.back();
        nodes.pop_back();

        const aiMatrix4x4 nodeTransform =
            entry.m_parentTransform * entry.m_node->mTransformation;

        for (unsigned int meshSlot = 0; meshSlot < entry.m_node->mNumMeshes; ++meshSlot) {
            const unsigned int meshIndex = entry.m_node->mMeshes[meshSlot];
            if (!geometryCache[meshIndex]) {
                geometryCache[meshIndex] = importMeshGeometry(
                    scene->mMeshes[meshIndex],
                    scene,
                    modelDirectory,
                    textureCache);
            }

            auto renderable = std::make_shared<MeshRenderable>(geometryCache[meshIndex]);
            renderable->setModelMatrix(toGlmMatrix(nodeTransform));
            renderables.push_back(std::move(renderable));
        }

        for (unsigned int childIndex = 0; childIndex < entry.m_node->mNumChildren; ++childIndex) {
            nodes.push_back({entry.m_node->mChildren[childIndex], nodeTransform});
        }
    }

    return renderables;
}
