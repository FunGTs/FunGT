#include "primitives.hpp"

Primitive::Primitive(){

}

Primitive::~Primitive()
{
}
void Primitive::set(const PrimitiveVertex *vertices, const unsigned numOfvert, const GLuint *indices, const unsigned numOfindices){

    for(size_t i = 0; i<numOfvert; i++){
        //use size_t for array indexing and loop counting
        this->m_vertex.push_back(vertices[i]);
    }
    for(size_t i = 0; i<numOfindices; i++){
        //use size_t for array indexing and loop counting
        this->m_index.push_back(indices[i]);
    }


}
void Primitive::set(const PrimitiveVertex *vertices, const unsigned numOfvert)
{
    for(size_t i = 0; i<numOfvert; i++){
        //use size_t for array indexing and loop counting
        this->m_vertex.push_back(vertices[i]);
    }
}
PrimitiveVertex *Primitive::getVertices()
{
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
    return sizeof(PrimitiveVertex)*this->m_vertex.size();
}
long unsigned Primitive::sizeOfIndices(){
    return sizeof(PrimitiveVertex)*this->m_index.size();
}

void Primitive::setAttribs()
{
    //Set Vertex Attributes pointers and enable n
    //glVertexAttribPointer(0 /*First element: positions*/,3 /* 3 floats*/, GL_FLOAT/*Type*/,GL_FALSE, 3*sizeof(GLfloat)/*how much steps to the next vertex pos*/, (GLvoid*)0);
    //glEnableVertexAttribArray(0); 
    //SET VERTEXATTRIBPOINTERS AND ENABLE (INPUT ASSEMBLY)
        //POSITION 
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(PrimitiveVertex),(GLvoid*)offsetof(PrimitiveVertex,position));
       
        //COLOR
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(PrimitiveVertex),(GLvoid*)offsetof(PrimitiveVertex,normal));
        
        //TEXTURE COORDS
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(PrimitiveVertex),(GLvoid*)offsetof(PrimitiveVertex,texcoord));
        
}

void Primitive::unsetAttribs()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

const std::vector<PrimitiveVertex>& Primitive::getVertices() const
{
    // TODO: insert return statement here
    return m_vertex;
}

const std::vector<unsigned int>& Primitive::getIndices() const
{
    // TODO: insert return statement here
    return m_index;
}

// Graphics initialization
void Primitive::setTexture(const std::string &pathToTexture)
{
    texture.genTexture(pathToTexture);
    texture.active();
    texture.bind();
}

void Primitive::InitGraphics()
{
    m_vao.genVAO();
    m_vb.genVB();
    m_vi.genVI();

    m_vao.bind();

    m_vb.bind();
    m_vb.bufferData(this->getVertices(), this->sizeOfVertices());

    this->setAttribs();

    // Upload index data if indices exist
    if (this->getNumOfIndices() > 0) {
        m_vi.bind();
        m_vi.indexData(this->getIndices(), this->sizeOfIndices());
    }

    m_vao.unbind();
    this->unsetAttribs();
    m_vb.unbind();
    if (this->getNumOfIndices() > 0) {
        m_vi.unbind();
    }
}
