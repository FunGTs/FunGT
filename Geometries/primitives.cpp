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
void Primitive::set(const Vertex *vertices, const unsigned numOfvert, const GLuint *indices, const unsigned numOfindices){

    for(size_t i = 0; i<numOfvert; i++){
        //use size_t for array indexing and loop counting
        this->m_vertex.push_back(vertices[i]);
    }
    for(size_t i = 0; i<numOfindices; i++){
        //use size_t for array indexing and loop counting
        this->m_index.push_back(indices[i]);
    }


}
void Primitive::set(const Vertex *vertices, const unsigned numOfvert)
{
    for(size_t i = 0; i<numOfvert; i++){
        //use size_t for array indexing and loop counting
        this->m_vertex.push_back(vertices[i]);
    }
}
Vertex *Primitive::getVertices()
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
    return sizeof(Vertex)*this->m_vertex.size();
}
long unsigned Primitive::sizeOfIndices(){
    return sizeof(Vertex)*this->m_index.size();
}

void Primitive::setAttribs()
{
    //Set Vertex Attributes pointers and enable n
    //glVertexAttribPointer(0 /*First element: positions*/,3 /* 3 floats*/, GL_FLOAT/*Type*/,GL_FALSE, 3*sizeof(GLfloat)/*how much steps to the next vertex pos*/, (GLvoid*)0);
    //glEnableVertexAttribArray(0); 
    //SET VERTEXATTRIBPOINTERS AND ENABLE (INPUT ASSEMBLY)
        //POSITION 
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,position));
       
        //COLOR
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,normal));
        
        //TEXTURE COORDS
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
        
}

void Primitive::unsetAttribs()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
