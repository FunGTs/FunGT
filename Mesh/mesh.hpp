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
#include "../include/prequisites.hpp"
#include "../VertexGL/vertexArrayObjects.hpp"
#include "../VertexGL/vertexBuffers.hpp"
#include "../VertexGL/vertexIndices.hpp"
#include "../Textures/textures.hpp"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)

class Mesh{
    
    public: 
        std::vector<funGTVERTEX> m_vertex; //An array of vertices
        std::vector<GLuint> m_index; // an array of indices 
        std::vector<Texture> m_texture; //An array of textures

    private: 
        //Render data: 
        VertexArrayObject m_vao; 
        VertexBuffer m_vb;
        VertexIndex m_vi; 
    
    public:
        Mesh();
        Mesh(std::vector<funGTVERTEX> inVertex,std::vector<GLuint> inIndex,std::vector<Texture> inTexture);
        ~Mesh();
        void initMesh();
        bool loadMesh(const std::string &filename);
        void draw(Shader &shader); 
        void render(); 
        bool initScene(const aiScene* pScene, const std::string& Filename); 


}; 

#endif // _MESH_H_
