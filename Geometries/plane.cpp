#include "plane.hpp"

Plane::Plane()
: Primitive(){
    printf("Plane default constructor\n");
}

Plane::~Plane()
{
    printf("USING Plane DESTRUCTOR\n");
}

void Plane::draw()
{
    texture.active();
    texture.bind();
    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, this->getNumOfVertices());
}

void Plane::setData()
{
    //     Vertex vertices[] = {
    //     // Positions          // Texture coordinates
    //     glm::vec3(1.0f, 0.0f, 1.0f),     glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f), // Top-right
    //     glm::vec3(1.0f, 0.0f, -1.0f),    glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f), // Bottom-right
    //     glm::vec3(-1.0f, 0.0f, -1.0f),   glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f), // Bottom-left
    //     glm::vec3(-1.0f, 0.0f, 1.0f),   glm::vec3(0.f, 0.f, 0.f), glm::vec2( 0.0f, 0.0f)  // Top-left
    // };
    PrimitiveVertex vertices[] = {
    // Positions             // Texture Coords
    glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(0.0f, 1.0f) ,
    glm::vec3(-1.0f, -1.0f, 0.0f),glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
    glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(1.0f, 0.0f),

    glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(0.0f, 1.0f) ,
    glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(1.0f, 0.0f) ,
     glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.f, 0.f, 0.f),glm::vec2(1.0f, 1.0f) 
};
    unsigned nOfvertices = sizeof(vertices)/sizeof(PrimitiveVertex);
    // GLuint indices[] = {
    //     0, 1, 3, // First triangle
    //     1, 2, 3  // Second triangle
    // };
        GLuint indices[] = {

        0, 1, 2,
        0 , 2, 3
    };
    unsigned nOfIndices = sizeof(indices)/sizeof(GLuint);
    this->set(vertices,nOfvertices);
}
