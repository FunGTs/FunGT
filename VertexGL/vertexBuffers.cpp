#include "vertexBuffers.hpp"

VertexBuffer::VertexBuffer(){

}
    
VertexBuffer::VertexBuffer(const void* data, unsigned int size){


    glGenBuffers(1,&id_Render);
    glBindBuffer(GL_ARRAY_BUFFER,id_Render);
    glBufferData(GL_ARRAY_BUFFER,size,data, GL_STATIC_DRAW);

}
VertexBuffer::~VertexBuffer(){
    glDeleteBuffers(1,&id_Render);
}
void VertexBuffer::genVB(const void* data, unsigned int size){
    glGenBuffers(1,&id_Render);
    glBindBuffer(GL_ARRAY_BUFFER,id_Render);
    glBufferData(GL_ARRAY_BUFFER,size,data, GL_STATIC_DRAW);

}
void VertexBuffer::build(){
    glBindBuffer(GL_ARRAY_BUFFER,id_Render);

}
void VertexBuffer::release(){
    glBindBuffer(GL_ARRAY_BUFFER,0);

}