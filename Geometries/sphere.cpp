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

    // Calculate number of triangles
    int triangleCount = m_sectorCount * m_stackCount * 2;
    glDrawArrays(GL_TRIANGLES, 0, triangleCount * 3);
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

    // Generate triangle indices
    std::vector<PrimitiveVertex> triangleVertices;

    for (int i = 0; i < m_stackCount; ++i) {
        int k1 = i * (m_sectorCount + 1);      // Beginning of current stack
        int k2 = k1 + m_sectorCount + 1;       // Beginning of next stack

        for (int j = 0; j < m_sectorCount; ++j, ++k1, ++k2) {
            // 2 triangles per sector excluding first and last stacks
            if (i != 0) {
                // Triangle 1 (k1, k2, k1+1)
                triangleVertices.push_back(vertices[k1]);
                triangleVertices.push_back(vertices[k2]);
                triangleVertices.push_back(vertices[k1 + 1]);
            }

            if (i != (m_stackCount - 1)) {
                // Triangle 2 (k1+1, k2, k2+1)
                triangleVertices.push_back(vertices[k1 + 1]);
                triangleVertices.push_back(vertices[k2]);
                triangleVertices.push_back(vertices[k2 + 1]);
            }
        }
    }

    unsigned nOfvertices = triangleVertices.size();
    this->set(triangleVertices.data(), nOfvertices);
}

void geometry::Sphere::IntancedDraw(Shader &shader, int instanceCount)
{

    texture.active();
    texture.bind();
    m_vao.bind();

    // Get vertex count from primitive!
    int vertexCount = getNumOfVertices();


    glDrawArraysInstanced(GL_TRIANGLES, 0, vertexCount, instanceCount);

    m_vao.unbind();
}
