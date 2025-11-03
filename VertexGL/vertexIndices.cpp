#include "vertexIndices.hpp"
VertexIndex::VertexIndex(){
    id_rnd = 0;
}
VertexIndex::VertexIndex(const unsigned int *data, unsigned int totIndices)
    : m_numOfIndices{ totIndices } {
    
    glGenBuffers(1,&id_rnd);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,id_rnd);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_numOfIndices * sizeof(GLuint), data, GL_STATIC_DRAW);

}
VertexIndex::~VertexIndex(){
    std::cout << "VIO Destructor, id_rnd=" << id_rnd << std::endl;
    if (id_rnd) {
        glDeleteBuffers(1, &id_rnd);
    }
    else if (id_rnd) {
        std::cerr << "[Warning] VIO::~VIO(): GL context not active, skipping glDeleteBuffers\n";
    }
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
    return m_numOfIndices;
}