#if !defined(_GEOMETRIES_H_)
#define _GEOMETRIES_H_
#include <vector>
#include "../Vertex/fungtVertex.hpp"
#include "../VertexGL/vertexArrayObjects.hpp"
#include "../VertexGL/vertexBuffers.hpp"
#include "../VertexGL/vertexIndices.hpp"
#include "../Textures/textures.hpp"
class Primitive{

private:
    std::vector<Vertex> m_vertex; 
    std::vector<GLuint> m_index;
protected:
    glm::mat4 m_ShapeModelMatrix = glm::mat4(1.f);
    glm::vec3 m_ShapePos = glm::vec3(1.0);
    glm::vec3 m_ShapeRot = glm::vec3(0.0);
    glm::vec3 m_ShapeScale = glm::vec3(0.5f);      

    
public:
    VertexArrayObject vao; 
    VertexBuffer vertexBuffer; 
    VertexIndex vertexIndex;
    Texture texture;
  

    public: 
        Primitive();
        Primitive(glm::vec3 shapePos);
        Primitive(float xpos, float ypos, float zpos);
        virtual ~Primitive();


        void set(const Vertex *vertices, const unsigned numOfvert, const GLuint *indices, const unsigned numOfindices);
        void set(const Vertex *vertices, const unsigned numOfvert);
        Vertex *getVertices();
        GLuint* getIndices();
        unsigned getNumOfVertices();
        unsigned getNumOfIndices();
        long unsigned sizeOfVertices();
        long unsigned sizeOfIndices();
        void setAttribs(); 
        void unsetAttribs(); 
        virtual void setData() = 0;
        virtual void create(const std::string &pathToTexture) = 0;
        virtual void draw() = 0;
        virtual glm::mat4 getModelMatrix() = 0;
        virtual void setPosition(glm::vec3 pos) = 0;   //Position 
        virtual void setModelMatrix() = 0;
        virtual void updateModelMatrix(float zrot) = 0;
        virtual void setScale(glm::vec3 scale) = 0;   


}; 



#endif // _GEOMETRIES_H_

