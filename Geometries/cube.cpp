#include "cube.hpp"

Cube::Cube()
: Primitive(){
    printf("USING CUBE\n");
}

Cube::~Cube()
{
    printf("USING CUBE DESTRUCTOR\n");
}

void Cube::draw(){
    texture.active();
    texture.bind();
    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Cube::setData()
{
    PrimitiveVertex vertices[] = {
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),

        glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),

        glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),

        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),

        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),

        glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f),
        glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 1.0f),
        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(1.0f, 0.0f),
        glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 0.0f),
        glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec2(0.0f, 1.0f)
    };

    unsigned nOfvertices = sizeof(vertices)/sizeof(PrimitiveVertex);

    this->set(vertices,nOfvertices);
}

void Cube::IntancedDraw(Shader& shader, int instanceCount)
{
    texture.active();
    texture.bind();
    m_vao.bind();

    int vertexCount = getNumOfVertices();
    glDrawArraysInstanced(GL_TRIANGLES, 0, vertexCount, instanceCount);

    m_vao.unbind();
}

