#include "square.hpp"

Square::Square()
: Primitive(){
}

Square::~Square(){
}

void Square::draw(){
    texture.active();
    texture.bind();
    m_vao.bind();
    glDrawElements(GL_TRIANGLES, this->getNumOfIndices(), GL_UNSIGNED_INT, 0);
}

void Square::setData()
{
    PrimitiveVertex vertices[] =
    {
        //POSITION                         //COLOR                  //Texcoords
        glm::vec3(-1.0f, 1.0f, 0.0f),     glm::vec3(1.f, 0.f, 0.f),   glm::vec2(0.f, 1.f),
        glm::vec3(-1.0f, -1.0f, 0.0f),    glm::vec3(0.f, 1.f, 0.f),   glm::vec2(0.f, 0.f),
        glm::vec3(1.0f, -1.0f, 0.0f),     glm::vec3(0.f, 0.f, 1.f),   glm::vec2(1.f, 0.f),
        glm::vec3(1.0f, 1.0f, 0.0f),      glm::vec3(1.f, 1.f, 0.f),   glm::vec2(1.f, 1.f)
    };
    unsigned nOfvertices = sizeof(vertices)/sizeof(PrimitiveVertex);

    GLuint indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    unsigned nOfIndices = sizeof(indices)/sizeof(GLuint);

    this->set(vertices, nOfvertices, indices, nOfIndices);
}