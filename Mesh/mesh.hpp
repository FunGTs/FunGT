#if !defined(_MESH_H_)
#define _MESH_H_
#include <assimp/Importer.hpp>   
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include<string>
#include<vector>
#include "../Shaders/shader.hpp"
#include "../Vertex/fungtVertex.hpp"
#include "../include/glmath.hpp"
#include "../include/prerequisites.hpp"
#include "../Textures/textures.hpp"
#include "../Material/material.hpp"
#include "../GraphicsRenderBackend/gpu_buffer.hpp"
#include <memory>
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices)
struct Texture_struct {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {

public:
    std::string m_name; // Name of the mesh
    std::vector<funGTVERTEX> m_vertex; //An array of vertices
    std::vector<unsigned int> m_index;// an array of indices
    std::vector<Texture> m_texture; //An array of texturesç
    std::vector<Material> m_material;
private:
    std::unique_ptr<GPUBuffer> m_buffer; // owns VAO + VBO + EBO

public:
    Mesh();
    Mesh(const std::vector<funGTVERTEX>& inVertex, const std::vector<uint32_t>& inIndex, std::vector<Texture> inTexture);
    Mesh(const std::vector<funGTVERTEX>& inVertex, const std::vector<uint32_t>& inIndex, const std::vector<Material>& inmaterial);
    Mesh(const std::vector<funGTVERTEX>& inVertex,
        const std::vector<uint32_t>& inIndex,
        std::vector<Texture> inTexture,
        const std::vector<Material>& inMaterial);  // takes texture by value — move into it
    ~Mesh();
    void initMesh();
    void InitOGLBuffers();
    void draw(Shader& shader);

};

#endif // _MESH_H_
