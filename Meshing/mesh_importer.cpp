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

std::shared_ptr<Texture> loadTexture(const std::string& path,
                                     TextureColorSpace colorSpace,
                                     TextureCache& textureCache)
{
    const std::string cacheKey = path +
        (colorSpace == TextureColorSpace::SRGB ? "#srgb" : "#linear");

    const auto found = textureCache.find(cacheKey);
    if (found != textureCache.end()) {
        return found->second;
    }

    auto texture = std::make_shared<Texture>();
    texture->genTexture(path, colorSpace);
    texture->setPath(path);

    textureCache[cacheKey] = texture;
    return texture;
}

std::shared_ptr<Texture> loadMaterialTexture(
    aiMaterial* assimpMaterial,
    aiTextureType textureType,
    const std::filesystem::path& modelDirectory,
    TextureColorSpace colorSpace,
    TextureCache& textureCache)
{
    if (assimpMaterial->GetTextureCount(textureType) == 0) {
        return nullptr;
    }

    aiString texturePath;
    if (assimpMaterial->GetTexture(textureType, 0, &texturePath) != AI_SUCCESS) {
        return nullptr;
    }

    const std::string relativePath = texturePath.C_Str();
    if (!relativePath.empty() && relativePath[0] == '*') {
        return nullptr;
    }

    const std::string fullPath =
        (modelDirectory / relativePath).lexically_normal().string();
    return loadTexture(fullPath, colorSpace, textureCache);
}

std::shared_ptr<Texture> loadBaseColorTexture(aiMaterial* assimpMaterial,
                                              const std::filesystem::path& modelDirectory,
                                              TextureCache& textureCache)
{
    std::shared_ptr<Texture> texture = loadMaterialTexture(
        assimpMaterial,
        aiTextureType_BASE_COLOR,
        modelDirectory,
        TextureColorSpace::SRGB,
        textureCache);

    if (texture) {
        return texture;
    }

    return loadMaterialTexture(
        assimpMaterial,
        aiTextureType_DIFFUSE,
        modelDirectory,
        TextureColorSpace::SRGB,
        textureCache);
}
Material importMaterial(aiMaterial* assimpMaterial,
                        const std::filesystem::path& modelDirectory,
                        TextureCache& textureCache)
{
    if (!assimpMaterial) {
        return Material::createDefaultMaterial();
    }

    aiString name;
    assimpMaterial->Get(AI_MATKEY_NAME, name);

    // Assimp can expose metallic/roughness defaults alongside this workflow.
    float glossiness = 1.0f;
    if (assimpMaterial->Get(AI_MATKEY_GLOSSINESS_FACTOR, glossiness) == AI_SUCCESS) {
        aiColor3D diffuseColor(1.0f, 1.0f, 1.0f);
        aiColor3D specularColor(1.0f, 1.0f, 1.0f);
        assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
        assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);

        const glm::vec3 baseColor(diffuseColor.r, diffuseColor.g, diffuseColor.b);
        const float roughness = glm::clamp(1.0f - glossiness, 0.05f, 1.0f);
        const float reflectance = glm::clamp(
            (specularColor.r + specularColor.g + specularColor.b) / 3.0f,
            0.0f,
            1.0f);

        Material material(baseColor, 0.0f, roughness, reflectance, name.C_Str());
        material.m_baseColorTexture = loadMaterialTexture(
            assimpMaterial,
            aiTextureType_DIFFUSE,
            modelDirectory,
            TextureColorSpace::SRGB,
            textureCache);
        material.m_normalTexture = loadMaterialTexture(
            assimpMaterial,
            aiTextureType_NORMALS,
            modelDirectory,
            TextureColorSpace::Linear,
            textureCache);
        material.m_occlusionTexture = loadMaterialTexture(
            assimpMaterial,
            aiTextureType_AMBIENT_OCCLUSION,
            modelDirectory,
            TextureColorSpace::Linear,
            textureCache);
        return material;
    }

    aiColor4D pbrBaseColor(1.0f, 1.0f, 1.0f, 1.0f);
    float pbrMetallic = 1.0f;
    float pbrRoughness = 1.0f;

    const bool isPBR =
        assimpMaterial->GetTextureCount(aiTextureType_BASE_COLOR) > 0 ||
        assimpMaterial->GetTextureCount(aiTextureType_METALNESS) > 0 ||
        assimpMaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0 ||
        assimpMaterial->Get(AI_MATKEY_BASE_COLOR, pbrBaseColor) == AI_SUCCESS ||
        assimpMaterial->Get(AI_MATKEY_METALLIC_FACTOR, pbrMetallic) == AI_SUCCESS ||
        assimpMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, pbrRoughness) == AI_SUCCESS;


    if (isPBR) {
        assimpMaterial->Get(AI_MATKEY_BASE_COLOR, pbrBaseColor);
        assimpMaterial->Get(AI_MATKEY_METALLIC_FACTOR, pbrMetallic);
        assimpMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, pbrRoughness);

        Material material(
            glm::vec3(pbrBaseColor.r, pbrBaseColor.g, pbrBaseColor.b),
            pbrMetallic,
            pbrRoughness,
            0.04f,
            name.C_Str());
        material.m_baseColorTexture = loadBaseColorTexture(
            assimpMaterial,
            modelDirectory,
            textureCache);
        material.m_metallicRoughnessTexture = loadMaterialTexture(
            assimpMaterial,
            aiTextureType_METALNESS,
            modelDirectory,
            TextureColorSpace::Linear,
            textureCache);
        material.m_normalTexture = loadMaterialTexture(
            assimpMaterial,
            aiTextureType_NORMALS,
            modelDirectory,
            TextureColorSpace::Linear,
            textureCache);
        material.m_occlusionTexture = loadMaterialTexture(
            assimpMaterial,
            aiTextureType_AMBIENT_OCCLUSION,
            modelDirectory,
            TextureColorSpace::Linear,
            textureCache);
        return material;
    }

    aiColor3D diffuseColor(0.8f, 0.8f, 0.8f);
    float shininess = 2.0f;
    float indexOfRefraction = 1.5f;

    assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
    assimpMaterial->Get(AI_MATKEY_SHININESS, shininess);
    assimpMaterial->Get(AI_MATKEY_REFRACTI, indexOfRefraction);

    if (indexOfRefraction <= 1.0f) {
        indexOfRefraction = 1.5f;
    }

    const glm::vec3 baseColor(diffuseColor.r, diffuseColor.g, diffuseColor.b);
    const float roughness = glm::clamp(std::sqrt(2.0f / (shininess + 2.0f)), 0.05f, 1.0f);
    const float iorRatio =
        (indexOfRefraction - 1.0f) / (indexOfRefraction + 1.0f);
    const float reflectance = glm::clamp(iorRatio * iorRatio, 0.0f, 1.0f);

    Material material(baseColor, 0.0f, roughness, reflectance, name.C_Str());
    material.m_baseColorTexture = loadBaseColorTexture(
        assimpMaterial,
        modelDirectory,
        textureCache);
    return material;
}

std::shared_ptr<MeshGeometry> importMeshGeometry(aiMesh* assimpMesh,
                                                 const aiScene* scene,
                                                 const aiMatrix4x4& nodeTransform,
                                                 const std::filesystem::path& modelDirectory,
                                                 TextureCache& textureCache)
{
    auto geometry = std::make_shared<MeshGeometry>();
    geometry->m_name = assimpMesh->mName.C_Str();
    geometry->m_positions.reserve(assimpMesh->mNumVertices);
    geometry->m_faceOffsets.push_back(0);

    const glm::mat4 transform = toGlmMatrix(nodeTransform);
    const glm::mat3 normalTransform =
        glm::transpose(glm::inverse(glm::mat3(transform)));

    for (unsigned int vertexIndex = 0; vertexIndex < assimpMesh->mNumVertices; ++vertexIndex) {
        const aiVector3D& position = assimpMesh->mVertices[vertexIndex];
        const glm::vec4 transformedPosition =
            transform * glm::vec4(position.x, position.y, position.z, 1.0f);
        geometry->m_positions.emplace_back(transformedPosition);
    }

    const bool hasVertexColors = assimpMesh->HasVertexColors(0);
    for (unsigned int faceIndex = 0; faceIndex < assimpMesh->mNumFaces; ++faceIndex) {
        const aiFace& face = assimpMesh->mFaces[faceIndex];

        for (unsigned int cornerIndex = 0; cornerIndex < face.mNumIndices; ++cornerIndex) {
            const uint32_t vertexIndex = face.mIndices[cornerIndex];
            geometry->m_cornerVertices.push_back(vertexIndex);

            if (assimpMesh->HasNormals()) {
                const aiVector3D& normal = assimpMesh->mNormals[vertexIndex];
                const glm::vec3 transformedNormal = glm::normalize(
                    normalTransform * glm::vec3(normal.x, normal.y, normal.z));
                geometry->m_cornerNormals.push_back(transformedNormal);
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

std::shared_ptr<MeshRenderable> importStaticMesh(const std::string& path)
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
    std::vector<std::shared_ptr<MeshGeometry>> geometries;
    std::vector<NodeEntry> nodes;

    nodes.push_back({scene->mRootNode, aiMatrix4x4()});

    while (!nodes.empty()) {
        const NodeEntry entry = nodes.back();
        nodes.pop_back();

        const aiMatrix4x4 nodeTransform =
            entry.m_parentTransform * entry.m_node->mTransformation;

        for (unsigned int meshSlot = 0; meshSlot < entry.m_node->mNumMeshes; ++meshSlot) {
            const unsigned int meshIndex = entry.m_node->mMeshes[meshSlot];
            geometries.push_back(importMeshGeometry(
                scene->mMeshes[meshIndex],
                scene,
                nodeTransform,
                modelDirectory,
                textureCache));
        }

        for (unsigned int childIndex = 0; childIndex < entry.m_node->mNumChildren; ++childIndex) {
            nodes.push_back({entry.m_node->mChildren[childIndex], nodeTransform});
        }
    }

    if (geometries.empty()) {
        throw std::runtime_error("The selected model contains no renderable meshes");
    }

    return std::make_shared<MeshRenderable>(std::move(geometries));
}
