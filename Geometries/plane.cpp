#include "plane.hpp"

Plane::Plane(float width, float depth)
    : Primitive(), m_width(width), m_depth(depth) {
    printf("Plane constructor: %.2f x %.2f\n", width, depth);
}

Plane::~Plane() {
    printf("USING Plane DESTRUCTOR\n");
}

void Plane::draw() {
    texture.active();
    texture.bind();
    m_vao.bind();
    glDrawElements(GL_TRIANGLES, this->getNumOfIndices(), GL_UNSIGNED_INT, 0);
    
}

void Plane::setData() {
    float halfWidth = m_width / 2.0f;
    float halfDepth = m_depth / 2.0f;

    PrimitiveVertex vertices[] = {
        // Horizontal plane (XZ) with normals pointing up (+Y)
        glm::vec3(-halfWidth, 0.0f,  halfDepth),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f), // Top-left
        glm::vec3(halfWidth, 0.0f,  halfDepth),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f), // Top-right
        glm::vec3(halfWidth, 0.0f, -halfDepth),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), // Bottom-right
        glm::vec3(-halfWidth, 0.0f, -halfDepth),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)  // Bottom-left
    };

    unsigned nOfVertices = sizeof(vertices) / sizeof(PrimitiveVertex);

    GLuint indices[] = {
        0, 1, 2,  // First triangle
        0, 2, 3   // Second triangle
    };

    unsigned nOfIndices = sizeof(indices) / sizeof(GLuint);

    this->set(vertices, nOfVertices, indices, nOfIndices);
}