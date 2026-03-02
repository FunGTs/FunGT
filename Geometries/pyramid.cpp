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
void Primitive::drawWireframe()
{
  if (m_vertex.empty()) return;

  // Save current polygon mode
  GLint polygonMode[2];
  glGetIntegerv(GL_POLYGON_MODE, polygonMode);

  // Draw as wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glLineWidth(2.0f);

  m_vao.bind();

  if (!m_index.empty()) {
    glDrawElements(GL_TRIANGLES, m_index.size(), GL_UNSIGNED_INT, 0);
  }
  else {
    glDrawArrays(GL_TRIANGLES, 0, m_vertex.size());
  }

  m_vao.unbind();

  // Restore polygon mode
  glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
  glLineWidth(1.0f);
}

void Primitive::drawVertices()
{
  if (m_vertex.empty()) return;

  glPointSize(8.0f);

  m_vao.bind();
  glDrawArrays(GL_POINTS, 0, m_vertex.size());
  m_vao.unbind();

  glPointSize(1.0f);
}