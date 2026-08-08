#if !defined(_INFINITE_GRID_H_)
#define _INFINITE_GRID_H_

#include "Renderable/renderable.hpp"
#include "Shaders/shader.hpp"
#include "../GraphicsRenderBackend/gpu_buffer.hpp"
#include <memory>

class InfiniteGrid : public Renderable {
private:
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<GPUBuffer> m_buffer;
    float m_nearPlane;
    float m_farPlane;
    glm::mat4 m_viewMatrix = glm::mat4(1.f);
    glm::mat4 m_projectionMatrix = glm::mat4(1.f);

public:
    InfiniteGrid();
    ~InfiniteGrid() override;

    void init(const std::string& vsPath, const std::string& fsPath);

    // Implement Renderable interface
    void draw() override;
    Shader& getShader() override { return *m_shader; }

    void setViewMatrix(const glm::mat4& viewMatrix) override {
        m_viewMatrix = viewMatrix;
    }

    glm::mat4 getViewMatrix() override {
        return m_viewMatrix;
    }
    glm::mat4 getModelMatrix() const override {
        return glm::mat4(1.0f);  // ← Identity = no transformation
    }
    void setProjectionMatrix(const glm::mat4& projMatrix) {
        m_projectionMatrix = projMatrix;
    }

    glm::mat4 getProjectionMatrix() override {
        return m_projectionMatrix;
    }

    void setPlanes(float near, float far) {
        m_nearPlane = near;
        m_farPlane = far;
    }
};

#endif // _INFINITE_GRID_H_