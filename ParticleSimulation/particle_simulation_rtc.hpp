#if !defined(_PARTICLE_SIM_RTC_H_)
#define _PARTICLE_SIM_RTC_H_
#include <GL/glew.h>
#include "particle.hpp"
#include "Renderable/renderable.hpp"
#include "VertexGL/vertexArrayObjects.hpp"
#include "VertexGL/vertexBuffers.hpp"
#include "VertexGL/vertexIndices.hpp"
#include "Shaders/shader.hpp"
#include "Path_Manager/path_manager.hpp"
#include <string>
#include <atomic>

class ParticleRTC : public Renderable {

    VertexArrayObject m_vao;
    VertexBuffer m_vbo;
    Shader m_shader;

    size_t m_NumParticles;
    float m_deltaTime = 0.005f;
    float x_pos = 0.0;
    float y_pos = 0.0;

    glm::vec3 m_position = glm::vec3(0.f);
    glm::vec3 m_rotation = glm::vec3(0.f);
    glm::vec3 m_scale = glm::vec3(1.0);
    glm::mat4 m_viewMatrix = glm::mat4(1.f);
    glm::mat4 m_projectionMatrix = glm::mat4(1.f);
    glm::mat4 m_ModelMatrix = glm::mat4(1.f);

    std::string m_vs, m_fs;
    std::atomic<bool> m_isReady{ false };

public:
    fgt::ParticleSet<float> m_pSet;

    ParticleRTC(std::size_t numOfParticles);

    bool compileInitKernel(const std::string& user_init_code, std::string& error_msg);
    void executeInitKernel();
    bool compileKernel(const std::string& user_code, std::string& error_msg);
    void executeKernel();
    static bool isSupported();
    bool hasKernel() const;
    bool hasInitKernel() const;

    void init();
    void simulation();

    void draw() override;
    Shader& getShader() override;
    void updateTime(float deltaTime) override;
    void setViewMatrix(const glm::mat4& viewMatrix) override;
    glm::mat4 getViewMatrix() override;
    void updateModelMatrix() override;
    glm::mat4 getModelMatrix() const override;
};


#endif // _PARTICLE_SIM_RTC_H_
