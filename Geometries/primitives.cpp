#include "primitives.hpp"

Primitive::Primitive(){

}
Primitive::~Primitive(){

}
void Primitive::set(const Vertex *vertices, const unsigned numOfvert, const GLuint *indices, const unsigned numOfindices){

    for(size_t i = 0; i<numOfvert; i++){
        //use size_t for array indexing and loop counting
        this->m_vertex.push_back(vertices[i]);
    }
    for(size_t i = 0; i<numOfindices; i++){
        //use size_t for array indexing and loop counting
        this->m_index.push_back(indices[i]);
    }


}
Vertex* Primitive::getVertices(){
    return this->m_vertex.data();
}
GLuint* Primitive::getIndices(){
    return this->m_index.data();
}
 unsigned Primitive::getNumOfVertices(){
    return this->m_vertex.size();
}
 unsigned Primitive::getNumOfIndices(){
    return this->m_index.size();
}
long unsigned Primitive::sizeOfVertices(){
    return sizeof(Vertex)*this->m_vertex.size();
}
long unsigned Primitive::sizeOfIndices(){
    return sizeof(Vertex)*this->m_index.size();
}
