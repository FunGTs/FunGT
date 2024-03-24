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
    
public:
    VertexArrayObject vao; 
    VertexBuffer vertexBuffer; 
    VertexIndex vertexIndex;
    Texture texture;   


    public: 
        Primitive();
        virtual ~Primitive();


        void set(const Vertex *vertices, const unsigned numOfvert, const GLuint *indices, const unsigned numOfindices);
        Vertex *getVertices();
        GLuint* getIndices();
        unsigned getNumOfVertices();
        unsigned getNumOfIndices();
        long unsigned sizeOfVertices();
        long unsigned sizeOfIndices();


}; 



#endif // _GEOMETRIES_H_

