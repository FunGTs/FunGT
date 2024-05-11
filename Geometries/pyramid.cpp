#include "pyramid.hpp"

Pyramid::Pyramid()
: Primitive(){
}
Pyramid::Pyramid(glm::vec3 pos)
: Primitive(pos){
}
Pyramid::Pyramid(float x, float y, float z)
: Primitive(x,y,z){
}
Pyramid::~Pyramid()
{
}
void Pyramid::draw(){
    texture.active();
    texture.bind();
    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 18);
}

void Pyramid::create(const std::string &path)
{
    this->setData();

    m_vao.genVAO(); //Generates a Vertex array object
    m_vb.genVB(); //Generates the Vertex Buffer
    m_vi.genVI(); //Generates the Vertex Buffer

    m_vao.bind();

    m_vb.bind();
    m_vb.bufferData(this->getVertices(),this->sizeOfVertices());
    
    this->setAttribs();
    
    texture.genTexture(path);
    //std::cout<<"Texture Path : "<<texture.getPath()<<std::endl; 
    texture.bind();

    //All binded above must be released
    m_vao.unbind();
    this->unsetAttribs();
    m_vb.unbind();
}

void Pyramid::setData()
{
      PrimitiveVertex vertices[] = {
        //Front face
        glm::vec3(0.0f, 1.0f, 0.0f),glm::vec3(0.f, 0.f, 0.f), glm::vec2( 0.5f, 1.0f),
        glm::vec3( -1.0f, -1.0f, 1.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(0.0f, 0.0f),
        glm::vec3( 1.0f, -1.0f, 1.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(1.0f, 0.0f),
        //Right face
        glm::vec3( 0.0f, 1.0f, 0.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2( 0.5f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),glm::vec3(0.f, 0.f, 0.f),glm::vec2( 0.0f, 0.0f),
        glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(1.0f, 0.0f),
        //Back face
        glm::vec3(0.0f, 1.0f, 0.0f),glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.5f, 1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(0.0f, 0.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(1.0f, 0.0f),
        //Left
        glm::vec3(0.0f, 1.0f, 0.0f),glm::vec3(0.f, 0.f, 0.f), glm::vec2( 0.5f, 1.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(0.0f, 0.0f),
        glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(1.0f, 0.0f),
        //Bottom face
        glm::vec3(-1.0f, -1.0f, 1.0f),glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(1.0f, 0.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.f, 0.0f),
        
        glm::vec3(-1.0f, -1.0f, 1.0f),glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(0.0f, 0.0f)
    };
    
      unsigned nOfvertices = sizeof(vertices)/sizeof(PrimitiveVertex);
  
    this->set(vertices,nOfvertices);
}

glm::mat4 Pyramid::getModelMatrix()
{
     return this->m_ShapeModelMatrix;
}

void Pyramid::setPosition(glm::vec3 pos)
{
    m_ShapePos = pos; 
}

void Pyramid::setModelMatrix()
{
   m_ShapeModelMatrix = glm::translate(m_ShapeModelMatrix, m_ShapePos);
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.x), glm::vec3(1.f, 0.f, 0.f));
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.y), glm::vec3(0.f, 1.f, 0.f)); 
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.z), glm::vec3(0.f, 0.f, 1.f));
    m_ShapeModelMatrix = glm::scale(m_ShapeModelMatrix, m_ShapeScale);
    
}

void Pyramid::updateModelMatrix(float zrot)
{
   m_ShapeRot.y = zrot;
    m_ShapeModelMatrix = glm::mat4(1.f); 
    m_ShapeModelMatrix = glm::translate(m_ShapeModelMatrix, m_ShapePos);
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.x), glm::vec3(1.f, 0.f, 0.f));
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.y), glm::vec3(0.f, 1.f, 0.f)); 
    m_ShapeModelMatrix = glm::rotate(m_ShapeModelMatrix, glm::radians(m_ShapeRot.z), glm::vec3(0.f, 0.f, 1.f));
    m_ShapeModelMatrix = glm::scale(m_ShapeModelMatrix, m_ShapeScale);
}
