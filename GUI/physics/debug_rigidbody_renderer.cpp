#include "debug_rigidbody_renderer.hpp"
#include <iostream>

DebugRenderer::DebugRenderer()
    : m_vao(0), m_vbo(0), m_initialized(false)
{
}

DebugRenderer::~DebugRenderer() {
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
}

void DebugRenderer::init() {
    // Use YOUR Shader class to create the shader program
    std::string vsPath = getAssetPath("shaders/debug_line_vs.glsl");
    std::string fsPath = getAssetPath("shaders/debug_line_fs.glsl");
    m_lineShader.create(vsPath, fsPath);

    // Setup VAO/VBO for line rendering
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    m_initialized = true;
    std::cout << "DebugRenderer initialized with shader" << std::endl;
}

void DebugRenderer::drawWireframeBox(
    const fungt::Vec3& center,
    const fungt::Vec3& size,
    const fungt::Vec3& color)
{
    // Calculate 8 corners of the box
    float halfW = size.x / 2.0f;
    float halfH = size.y / 2.0f;
    float halfD = size.z / 2.0f;

    fungt::Vec3 corners[8] = {
        // Bottom face (y = center.y - halfH)
        center + fungt::Vec3(-halfW, -halfH, -halfD),  // 0: left-bottom-front
        center + fungt::Vec3(halfW, -halfH, -halfD),  // 1: right-bottom-front
        center + fungt::Vec3(halfW, -halfH,  halfD),  // 2: right-bottom-back
        center + fungt::Vec3(-halfW, -halfH,  halfD),  // 3: left-bottom-back

        // Top face (y = center.y + halfH)
        center + fungt::Vec3(-halfW,  halfH, -halfD),  // 4: left-top-front
        center + fungt::Vec3(halfW,  halfH, -halfD),  // 5: right-top-front
        center + fungt::Vec3(halfW,  halfH,  halfD),  // 6: right-top-back
        center + fungt::Vec3(-halfW,  halfH,  halfD)   // 7: left-top-back
    };

    // Define 12 edges
    int edges[12][2] = {
        // Bottom face edges
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        // Top face edges
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        // Vertical edges
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    // Add vertices to buffer (position + color for each vertex)
    for (int i = 0; i < 12; i++) {
        // Start vertex
        m_vertices.push_back(corners[edges[i][0]].x);
        m_vertices.push_back(corners[edges[i][0]].y);
        m_vertices.push_back(corners[edges[i][0]].z);
        m_vertices.push_back(color.x);
        m_vertices.push_back(color.y);
        m_vertices.push_back(color.z);

        // End vertex
        m_vertices.push_back(corners[edges[i][1]].x);
        m_vertices.push_back(corners[edges[i][1]].y);
        m_vertices.push_back(corners[edges[i][1]].z);
        m_vertices.push_back(color.x);
        m_vertices.push_back(color.y);
        m_vertices.push_back(color.z);
    }
}

void DebugRenderer::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!m_initialized || m_vertices.empty()) return;

    // Disable depth test so lines are always visible
    //glDisable(GL_DEPTH_TEST);
    glLineWidth(3.0f);

    // Use YOUR Shader class methods
    m_lineShader.Bind();

    // Set uniforms using YOUR Shader class methods
    m_lineShader.setUniformMat4fv("u_View", view);
    m_lineShader.setUniformMat4fv("u_Projection", projection);

    // Upload vertex data
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_DYNAMIC_DRAW);
    std::cout << "DREW " << (m_vertices.size() / 6) << " VERTICES!" << std::endl;  // ← ADD THIS
    // Draw lines
    glDrawArrays(GL_LINES, 0, m_vertices.size() / 6);

    glBindVertexArray(0);
    m_lineShader.unBind();

    glEnable(GL_DEPTH_TEST);
}

void DebugRenderer::clear() {
    m_vertices.clear();
}