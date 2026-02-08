#if !defined(_DEBUG_RENDERER_H_)
#define _DEBUG_RENDERER_H_

#include <GL/glew.h>
#include <vector>
#include "Shaders/shader.hpp"
#include "Vector/vector3.hpp"
#include "Path_Manager/path_manager.hpp"
class DebugRenderer {
private:
    GLuint m_vao;
    GLuint m_vbo;
    Shader m_lineShader;  // Use YOUR Shader class!

    std::vector<float> m_vertices;  // Line vertices (pos + color)
    bool m_initialized;

public:
    DebugRenderer();
    ~DebugRenderer();

    // Initialize with shader paths
    void init();

    // Draw wireframe box
    void drawWireframeBox(
        const fungt::Vec3& center,
        const fungt::Vec3& size,
        const fungt::Vec3& color = fungt::Vec3(1.0f, 0.0f, 0.0f)
    );

    // Render all queued lines
    void render(const glm::mat4& view, const glm::mat4& projection);

    // Clear queued lines
    void clear();
};

#endif // _DEBUG_RENDERER_H_