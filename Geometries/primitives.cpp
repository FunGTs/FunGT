#include "primitives.hpp"

Primitive::Primitive(){

}
Primitive::Primitive(glm::vec3 shapePos)
{
    m_ShapePos = shapePos; 
}
Primitive::Primitive(float xpos, float ypos, float zpos)
{
    m_ShapePos.x = xpos; 
    m_ShapePos.y = ypos; 
    m_ShapePos.z = zpos; 
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

// Renderable interface implementations
Shader& Primitive::getShader()
{
    return m_Shader;
}

glm::mat4 Primitive::getModelMatrix()
{
    return m_ShapeModelMatrix;
}

void Primitive::updateModelMatrix()
{
    m_ShapeModelMatrix = glm::mat4(1.f);
    m_ShapeModelMatrix = glm::translate(m_ShapeModelMatrix, m_ShapePos);
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.x), glm::vec3(1.f, 0.f, 0.f));
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.y), glm::vec3(0.f, 1.f, 0.f));
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.z), glm::vec3(0.f, 0.f, 1.f));
    m_ShapeModelMatrix = glm::scale(m_ShapeModelMatrix, m_ShapeScale);
}

// Transform setters
void Primitive::setPosition(glm::vec3 pos)
{
    m_ShapePos = pos;
}

void Primitive::setRotation(glm::vec3 rot)
{
    m_ShapeRot = rot;
}

void Primitive::setScale(glm::vec3 scale)
{
    m_ShapeScale = scale;
}
