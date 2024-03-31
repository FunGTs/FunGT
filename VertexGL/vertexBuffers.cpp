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
void VertexBuffer::genVB(){
    glGenBuffers(1,&id_Render);
    //glBindBuffer(GL_ARRAY_BUFFER,id_Render);
    //glBufferData(GL_ARRAY_BUFFER,size,data, GL_STATIC_DRAW);

}
void VertexBuffer::bind(){
    glBindBuffer(GL_ARRAY_BUFFER,id_Render);

}
void VertexBuffer::unbind(){
    glBindBuffer(GL_ARRAY_BUFFER,0);

}

void VertexBuffer::bufferData(const void *data, unsigned int size)
{
    glBufferData(GL_ARRAY_BUFFER,size,data, GL_STATIC_DRAW);
}
