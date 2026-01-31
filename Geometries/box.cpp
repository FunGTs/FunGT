#include "box.hpp"

geometry::Box::Box(float width, float height, float depth)
    : Primitive(), m_width(width), m_height(height), m_depth(depth) {
    printf("USING BOX\n");
}

geometry::Box::~Box() {
    printf("USING BOX DESTRUCTOR\n");
}

void geometry::Box::draw() {
    texture.active();
    texture.bind();
    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    m_vao.unbind();
}

void geometry::Box::setData() {
    std::cout << "Calling Box::setData() " <<std::endl;
    float w = m_width / 2.0f;
    float h = m_height / 2.0f;
    float d = m_depth / 2.0f;
    float testH = h ;  // 10 units higher than it should be
    PrimitiveVertex vertices[] = {
        // Back face (z = -d) - CCW from outside (looking at -Z)
        {{-w, -h, -d}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
        {{ w, -h, -d}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
        {{ w,  h, -d}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
        {{ w,  h, -d}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
        {{-w,  h, -d}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
        {{-w, -h, -d}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},

        // Front face (z = +d) - CCW from outside (looking at +Z)
        {{-w, -h,  d}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
        {{ w,  h,  d}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
        {{ w, -h,  d}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
        {{-w, -h,  d}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
        {{-w,  h,  d}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
        {{ w,  h,  d}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},

        // Left face (x = -w) - CCW from outside (looking at -X)
        {{-w, -h, -d}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{-w,  h, -d}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        {{-w,  h,  d}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{-w,  h,  d}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{-w, -h,  d}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{-w, -h, -d}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},

        // Right face (x = +w) - CCW from outside (looking at +X)
        {{ w, -h, -d}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{ w, -h,  d}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{ w,  h,  d}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{ w,  h,  d}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{ w,  h, -d}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        {{ w, -h, -d}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},

        // Bottom face (y = -h) - CCW from outside (looking at -Y)
        {{-w, -h, -d}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
        {{-w, -h,  d}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
        {{ w, -h,  d}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
        {{ w, -h,  d}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
        {{ w, -h, -d}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
        {{-w, -h, -d}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},

        // Top face (y = +h) - CCW from outside (looking at +Y)
     // Top face (y = +h)
        {{-w,  testH, -d}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ w,  testH, -d}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ w,  testH,  d}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
        {{ w,  testH,  d}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
        {{-w,  testH,  d}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
        {{-w,  testH, -d}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
    };

    unsigned nOfvertices = sizeof(vertices) / sizeof(PrimitiveVertex);
    std::cout << "Box::setData() - nOfvertices=" << nOfvertices << std::endl;
    this->set(vertices, nOfvertices);
}

void geometry::Box::IntancedDraw(Shader& shader, int instanceCount)
{
    shader.setUniformVec3f(glm::vec3(0.4f, 0.4f, 0.4f), "u_color");
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    texture.active();
    texture.bind();
    m_vao.bind();

    // Get vertex count from primitive!
    int vertexCount = getNumOfVertices();


    glDrawArraysInstanced(GL_TRIANGLES, 0, vertexCount, instanceCount);

    m_vao.unbind();

  //  glDisable(GL_CULL_FACE);
}
