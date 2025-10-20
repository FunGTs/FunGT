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
#include "../VertexGL/vertexArrayObjects.hpp"
#include "../VertexGL/vertexBuffers.hpp"
#include "../VertexGL/vertexIndices.hpp"
#include "../Textures/textures.hpp"
#include "../Material/material.hpp"
#include "../Triangle/triangle.hpp"
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices)
struct Texture_struct {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh{
    
    public: 
        std::vector<funGTVERTEX> m_vertex; //An array of vertices
        std::vector<unsigned int> m_index;// an array of indices 
        std::vector<Texture> m_texture; //An array of texturesç
        std::vector<Material> m_material; 
        //unsigned int VAO;
        VertexArrayObject m_vao; 

    private: 
        //Render data: 
       
        VertexBuffer m_vb;
        VertexIndex m_vi; 
       
         //unsigned int VBO, EBO; 
    
    public:
        Mesh();
        Mesh(const std::vector<funGTVERTEX> &inVertex,const std::vector<GLuint> &inIndex,const std::vector<Texture> &inTexture);
        Mesh(const std::vector<funGTVERTEX> &inVertex,const std::vector<GLuint> &inIndex,const std::vector<Material> &inmaterial);
        ~Mesh();
        void initMesh();
        void InitOGLBuffers();
        void draw(Shader &shader); 

        std::vector<Triangle> ToTriangle() const ;

}; 

#endif // _MESH_H_
