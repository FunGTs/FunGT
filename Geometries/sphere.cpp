#include "sphere.hpp"
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

geometry::Sphere::Sphere(float radius, int sectorCount, int stackCount)
    : Primitive(), m_radius(radius), m_sectorCount(sectorCount), m_stackCount(stackCount) {
    printf("USING SPHERE\n");
}

geometry::Sphere::~Sphere() {
    printf("USING SPHERE DESTRUCTOR\n");
}

void geometry::Sphere::draw() {
    texture.active();
    texture.bind();
    m_vao.bind();

    glDrawElements(GL_TRIANGLES, this->getNumOfIndices(), GL_UNSIGNED_INT, 0);
}
void geometry::Sphere::InstancedDraw(Shader& shader, int instanceCount)
{
    texture.active();
    texture.bind();
    m_vao.bind();

    glDrawElementsInstanced(GL_TRIANGLES, this->getNumOfIndices(), GL_UNSIGNED_INT, 0, instanceCount);

    m_vao.unbind();
}
void geometry::Sphere::setData() {
    std::cout << "Calling Sphere::setData() " << std::endl;
    std::vector<PrimitiveVertex> vertices;

    float sectorStep = 2 * M_PI / m_sectorCount;
    float stackStep = M_PI / m_stackCount;

    // Generate vertices for each stack and sector
    for (int i = 0; i <= m_stackCount; ++i) {
        float stackAngle = M_PI / 2 - i * stackStep;  // Starting from pi/2 to -pi/2
        float xy = m_radius * cosf(stackAngle);       // r * cos(u)
        float y = m_radius * sinf(stackAngle);        // r * sin(u)

        for (int j = 0; j <= m_sectorCount; ++j) {
            float sectorAngle = j * sectorStep;       // Starting from 0 to 2pi

            // Vertex position
            float x = xy * cosf(sectorAngle);         // r * cos(u) * cos(v)
            float z = xy * sinf(sectorAngle);         // r * cos(u) * sin(v)
            glm::vec3 position(x, y, z);

            // Normal (for sphere, normalized position is the normal)
            glm::vec3 normal = glm::normalize(position);

            // Texture coordinates
            float u = (float)j / m_sectorCount;
            float v = (float)i / m_stackCount;
            glm::vec2 texCoord(u, v);

            vertices.push_back({ position, normal, texCoord });
        }
    }

    std::vector<GLuint> indices;

    for (int i = 0; i < m_stackCount; ++i) {
        int k1 = i * (m_sectorCount + 1);
        int k2 = k1 + m_sectorCount + 1;

        for (int j = 0; j < m_sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (m_stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    this->set(vertices.data(), (unsigned)vertices.size(), indices.data(), (unsigned)indices.size());
}

