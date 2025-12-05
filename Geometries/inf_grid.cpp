#include "inf_grid.hpp"
#include <iostream>

InfiniteGrid::InfiniteGrid()
    : m_nearPlane(0.1f)
    , m_farPlane(500.0f)
    , m_viewMatrix(glm::mat4(1.0f))
    , m_projectionMatrix(glm::mat4(1.0f))
{
    // Generate empty VAO (shader generates vertices procedurally)
    glGenVertexArrays(1, &m_VAO);
    std::cout << "InfiniteGrid constructor" << std::endl;
}

InfiniteGrid::~InfiniteGrid() {
    glDeleteVertexArrays(1, &m_VAO);
    std::cout << "InfiniteGrid destructor" << std::endl;
}

void InfiniteGrid::init(const std::string& vsPath, const std::string& fsPath) {
    m_shader.create(vsPath, fsPath);
    std::cout << "Infinite grid shader created" << std::endl;
}

void InfiniteGrid::draw() {
    //m_shader.Bind();

    // Set uniforms
    m_shader.setUniformMat4fv("ViewMatrix", m_viewMatrix);
    m_shader.setUniformMat4fv("ProjectionMatrix", m_projectionMatrix);
    m_shader.setUniform1f(m_nearPlane, "nearPlane");
    m_shader.setUniform1f(m_farPlane, "farPlane");

    glBindVertexArray(m_VAO);

    // Enable blending for grid transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth writing (grid should not block objects)
    glDepthMask(GL_FALSE);

    // Draw 6 vertices (2 triangles = fullscreen quad)
    // Shader generates vertices procedurally
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Restore depth writing
    glDepthMask(GL_TRUE);

    glBindVertexArray(0);
    //m_shader.unBind();
}