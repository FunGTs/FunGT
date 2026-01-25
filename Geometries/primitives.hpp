#if !defined(_GEOMETRIES_H_)
#define _GEOMETRIES_H_
#include <vector>
#include "../include/prerequisites.hpp"
#include "../include/glmath.hpp"
#include "../VertexGL/vertexArrayObjects.hpp"
#include "../VertexGL/vertexBuffers.hpp"
#include "../VertexGL/vertexIndices.hpp"
#include "../Textures/textures.hpp"
#include "Shaders/shader.hpp"

struct PrimitiveVertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;

};


class Primitive {

private:
    std::vector<PrimitiveVertex> m_vertex;
    std::vector<GLuint> m_index;

public:
    VertexArrayObject m_vao;
    VertexBuffer m_vb;
    VertexIndex m_vi;
    Texture texture;


    public:
        Primitive();
        virtual ~Primitive();


        void set(const PrimitiveVertex *vertices, const unsigned numOfvert, const GLuint *indices, const unsigned numOfindices);
        void set(const PrimitiveVertex *vertices, const unsigned numOfvert);
        PrimitiveVertex *getVertices();
        GLuint* getIndices();
        unsigned getNumOfVertices();
        unsigned getNumOfIndices();
        long unsigned sizeOfVertices();
        long unsigned sizeOfIndices();
        void setAttribs();
        void unsetAttribs();

        // Geometry-specific virtuals
        virtual void setData() = 0;

        // Graphics initialization
        void setTexture(const std::string &pathToTexture);
        void InitGraphics();

        // Pure virtual draw method
        virtual void draw() = 0;
        virtual glm::mat4 getModelMatrix() const = 0;
        virtual void setPosition(glm::vec3 pos) = 0;   //Position 
        virtual void setModelMatrix() = 0;
        virtual void updateModelMatrix(float zrot) = 0;
        virtual void setScale(glm::vec3 scale) = 0;   


}; 



#endif // _GEOMETRIES_H_

