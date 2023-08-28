#include "primitives.hpp"

Primitive::Primitive(){

}
Primitive::~Primitive(){

}
void Primitive::set(const Vertex *vertices, const unsigned numOfvert, const GLuint *indices, const unsigned numOfindices){

    for(size_t i = 0; i<numOfvert; i++){
        //use size_t for array indexing and loop counting
        this->vertex.push_back(vertices[i]);
    }
    for(size_t i = 0; i<numOfindices; i++){
        //use size_t for array indexing and loop counting
        this->index.push_back(indices[i]);
    }


}
Vertex* Primitive::getVertices(){
    return this->vertex.data();
}
GLuint* Primitive::getIndices(){
    return this->index.data();
}
 unsigned Primitive::getNumOfVertices(){
    return this->vertex.size();
}
 unsigned Primitive::getNumOfIndices(){
    return this->index.size();
}
long unsigned Primitive::sizeOfVertices(){
    return sizeof(Vertex)*this->vertex.size();
}
long unsigned Primitive::sizeOfIndices(){
    return sizeof(Vertex)*this->vertex.size();
}
