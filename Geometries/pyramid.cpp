#include "pyramid.hpp"

Pyramid::Pyramid()
: Primitive(){
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
