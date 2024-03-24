#include "vertexArrayObjects.hpp"

VertexArrayObject::VertexArrayObject(){

}
VertexArrayObject::VertexArrayObject(int index){
    glGenVertexArrays(1,&id_vao);
    glBindVertexArray(id_vao);
}
VertexArrayObject::~VertexArrayObject(){
    glDeleteBuffers(1,&id_vao);
}
void VertexArrayObject::genVAO(){
    glGenVertexArrays(1,&id_vao);
    glBindVertexArray(id_vao);
}
void VertexArrayObject::bind(){
    glBindVertexArray(id_vao);
}
void VertexArrayObject::unbind(){
    glBindVertexArray(0);
}