#include "vertexArrayObjects.hpp"

VertexArrayObject::VertexArrayObject(){
    id_vao = 0; // Initialize id_vao to 0 by default
}
VertexArrayObject::VertexArrayObject(int index){
    glGenVertexArrays(1,&id_vao);
    glBindVertexArray(id_vao);
}
VertexArrayObject::~VertexArrayObject(){
    glDeleteVertexArrays(1, &id_vao);
}
void VertexArrayObject::genVAO(){
    glGenVertexArrays(1,&id_vao);
}
void VertexArrayObject::bind(){
    glBindVertexArray(id_vao);
}
void VertexArrayObject::unbind(){
    glBindVertexArray(0);
}