#include "vertexIndices.hpp"
VI::VI(){

}
VI::VI(const unsigned int *data, unsigned int totIndices)
: numId_rnd{totIndices}{

    glGenBuffers(1,&id_rnd);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,id_rnd);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,numId_rnd*sizeof(GLuint),data,GL_STATIC_DRAW);

}
VI::~VI(){
    glDeleteBuffers(1,&id_rnd);
}
void VI::genVI(const unsigned int *data, unsigned int totIndices){
    numId_rnd = totIndices;
    glGenBuffers(1,&id_rnd);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,id_rnd);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,numId_rnd*sizeof(GLuint),data,GL_STATIC_DRAW);

}
void VI::build(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,id_rnd);

}
void VI::release(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

}
unsigned int VI::getNumIndices() const {
    return numId_rnd;
}