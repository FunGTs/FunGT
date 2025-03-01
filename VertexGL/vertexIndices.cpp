#include "vertexIndices.hpp"
VertexIndex::VertexIndex(){

}
VertexIndex::VertexIndex(const unsigned int *data, unsigned int totIndices)
: numId_rnd{totIndices}{

    glGenBuffers(1,&id_rnd);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,id_rnd);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,numId_rnd*sizeof(GLuint),data,GL_STATIC_DRAW);

}
VertexIndex::~VertexIndex(){
    glDeleteBuffers(1,&id_rnd);
}
void VertexIndex::genVI(){
 
    glGenBuffers(1,&id_rnd);
    

}
void VertexIndex::bind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,id_rnd);

}
void VertexIndex::unbind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

}
void VertexIndex::indexData(const unsigned int *data, unsigned int sizeOfData)
{
    if (id_rnd == 0) {
        glGenBuffers(1, &id_rnd);
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeOfData,data,GL_STATIC_DRAW);   
}
unsigned int VertexIndex::getNumIndices() const
{
    return numId_rnd;
}