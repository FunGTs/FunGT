#include "vertexBuffers.hpp"

VB::VB(){

}

VB::VB(const void* data, unsigned int size){


    glGenBuffers(1,&id_Render);
    glBindBuffer(GL_ARRAY_BUFFER,id_Render);
    glBufferData(GL_ARRAY_BUFFER,size,data, GL_STATIC_DRAW);

}
VB::~VB(){
    glDeleteBuffers(1,&id_Render);
}
void VB::genVB(const void* data, unsigned int size){
    glGenBuffers(1,&id_Render);
    glBindBuffer(GL_ARRAY_BUFFER,id_Render);
    glBufferData(GL_ARRAY_BUFFER,size,data, GL_STATIC_DRAW);

}
void VB::build(){
    glBindBuffer(GL_ARRAY_BUFFER,id_Render);

}
void VB::release(){
    glBindBuffer(GL_ARRAY_BUFFER,0);

}