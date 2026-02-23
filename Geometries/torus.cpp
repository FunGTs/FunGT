#include "torus.hpp"
#include <cmath>

Torus::Torus(float majorRadius, float minorRadius, int majorSegments, int minorSegments)
    : Primitive(),
    m_majorRadius(majorRadius),
    m_minorRadius(minorRadius),
    m_majorSegments(majorSegments),
    m_minorSegments(minorSegments)
{
    std::cout << "Torus constructor" << std::endl;
}

Torus::~Torus()
{
    std::cout << "Torus destructor" << std::endl;
}

void Torus::setData()
{
    std::vector<PrimitiveVertex> vertices;
    std::vector<GLuint> indices;

    const float PI = 3.14159265359f;

    // Generate vertices
    for (int i = 0; i <= m_majorSegments; ++i) {
        float u = (float)i / (float)m_majorSegments * 2.0f * PI;
        float cosU = cos(u);
        float sinU = sin(u);

        for (int j = 0; j <= m_minorSegments; ++j) {
            float v = (float)j / (float)m_minorSegments * 2.0f * PI;
            float cosV = cos(v);
            float sinV = sin(v);

            // Position
            glm::vec3 pos;
            pos.x = (m_majorRadius + m_minorRadius * cosV) * cosU;
            pos.y = (m_majorRadius + m_minorRadius * cosV) * sinU;
            pos.z = m_minorRadius * sinV;

            // Normal (pointing outward from tube surface)
            glm::vec3 normal;
            normal.x = cosV * cosU;
            normal.y = cosV * sinU;
            normal.z = sinV;
            normal = glm::normalize(normal);

            // Texture coordinates
            glm::vec2 texcoord;
            texcoord.x = (float)i / (float)m_majorSegments;
            texcoord.y = (float)j / (float)m_minorSegments;

            PrimitiveVertex vertex;
            vertex.position = pos;
            vertex.normal = normal;
            vertex.texcoord = texcoord;

            vertices.push_back(vertex);
        }
    }

    // Generate indices (quads made of two triangles)
    for (int i = 0; i < m_majorSegments; ++i) {
        for (int j = 0; j < m_minorSegments; ++j) {
            int a = i * (m_minorSegments + 1) + j;
            int b = a + m_minorSegments + 1;
            int c = a + 1;
            int d = b + 1;

            // First triangle
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);

            // Second triangle
            indices.push_back(c);
            indices.push_back(b);
            indices.push_back(d);
        }
    }

    // Store in base class vectors
    PrimitiveVertex* vertArray = vertices.data();
    GLuint* indArray = indices.data();
    this->set(vertArray, vertices.size(), indArray, indices.size());

    std::cout << "Torus generated: " << vertices.size() << " vertices, "
        << indices.size() / 3 << " triangles" << std::endl;
}

void Torus::draw()
{
    texture.active();
    texture.bind();
    m_vao.bind();
    glDrawElements(GL_TRIANGLES, this->getNumOfIndices(), GL_UNSIGNED_INT, 0);
    m_vao.unbind();
}

void Torus::InstancedDraw(Shader& shader, int instanceCount)
{
    texture.active();
    texture.bind();
    m_vao.bind();
    glDrawElementsInstanced(GL_TRIANGLES, this->getNumOfIndices(), GL_UNSIGNED_INT, 0, instanceCount);
    m_vao.unbind();
}

void Torus::setRadii(float major, float minor)
{
    m_majorRadius = major;
    m_minorRadius = minor;
    setData();  // Regenerate geometry
    InitGraphics();  // Re-upload to GPU
}

void Torus::setSegments(int major, int minor)
{
    m_majorSegments = major;
    m_minorSegments = minor;
    setData();  // Regenerate geometry
    InitGraphics();  // Re-upload to GPU
}