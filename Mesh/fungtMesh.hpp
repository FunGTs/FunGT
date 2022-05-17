#if !defined(_MESH_FUNGT_G_)
#define _MESH_FUNGT_G_
#include<vector>
#include<iostream>
#include "../VertexGL/vertexArrayObjects.hpp"
#include "../VertexGL/vertexBuffers.hpp"
#include "../VertexGL/vertexIndices.hpp"
#include "../Material/material.hpp"
#include "../Vertex/fungtVertex.hpp"
namespace funGT{

    class Mesh{

        private:
            unsigned int numOfVertices; 
            unsigned int numOfIndices; 
            glm::vec3 position;
            glm::vec3 rotation;
            glm::vec3 scale;
            glm::mat4 ModelMatrix; 
            VAO vertexArrayObject;
            VB vertexBuffer;
            VI vertexIndices;

            void initVAO(VERTEX *vinput, const unsigned int &numOfVert, GLuint *indexArray, const unsigned int& numOfIndices); 
            void initModelMatrix();
            void updateUniform(Shader *shader);
            void updateModelMatrix();
        public:
            Mesh(VERTEX *vinput,  unsigned int &numOfVert, GLuint *indexArray, unsigned int& numOfIndices);
            Mesh();
            ~Mesh();
            void update();
            void render(Shader *shader /*core_program*/);
    };

}


#endif // _MESH_FUNGT_G_
