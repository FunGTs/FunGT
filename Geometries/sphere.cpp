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
    m_buffer->bindVAO();
    m_buffer->drawIndexed(getNumOfIndices());
    m_buffer->unbindVAO();
}

void geometry::Sphere::InstancedDraw(Shader& shader, int instanceCount) {
    texture.active();
    m_buffer->bindVAO();
    m_buffer->drawIndexedInstanced(getNumOfIndices(), instanceCount);
    m_buffer->unbindVAO();
}
void geometry::Sphere::setData() {
    std::cout << "Calling Sphere::setData() " << std::endl;
    std::vector<PrimitiveVertex> vertices;

    float sectorStep = 2 * M_PI / m_sectorCount;
    float stackStep = M_PI / m_stackCount;

    // Generate vertices for each stack and sector
    for (int i = 0; i <= m_stackCount; ++i) {
        float stackAngle = M_PI / 2 - i * stackStep;
        float xy = m_radius * cosf(stackAngle);
        float y = m_radius * sinf(stackAngle);

        for (int j = 0; j <= m_sectorCount; ++j) {
            float sectorAngle = j * sectorStep;

            float x = xy * cosf(sectorAngle);
            float z = xy * sinf(sectorAngle);
            glm::vec3 position(x, y, z);

            glm::vec3 normal = glm::normalize(position);

            float u = (float)j / m_sectorCount;
            float v = (float)i / m_stackCount;
            glm::vec2 texCoord(u, v);

            vertices.push_back({ position, normal, texCoord });
        }
    }

    std::vector<uint32_t> indices;

    for (int i = 0; i < m_stackCount; ++i) {
        int k1 = i * (m_sectorCount + 1);
        int k2 = k1 + m_sectorCount + 1;

        for (int j = 0; j < m_sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k1 + 1);
                indices.push_back(k2);
            }

            if (i != (m_stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2 + 1);
                indices.push_back(k2);
            }
        }
    }

    this->set(vertices.data(), (unsigned)vertices.size(), indices.data(), (unsigned)indices.size());
}

