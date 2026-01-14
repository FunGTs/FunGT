#if !defined(_PARTICLE_SIMULATION_H_)
#define _PARTICLE_SIMULATION_H_
#include "Renderable/renderable.hpp"
#include "VertexGL/vertexArrayObjects.hpp"
#include "VertexGL/vertexBuffers.hpp"
#include "VertexGL/vertexIndices.hpp"
#include "Shaders/shader.hpp"
#include "particle_demos.hpp"
#include <funlib/funlib.hpp>
class ParticleSimulation : public Renderable {
    
    VertexArrayObject m_vao; 
    VertexBuffer m_vbo; 
    Shader m_shader;
    size_t m_NumParticles; 
    float m_deltaTime = 2.0;
    float x_pos = 0.0;
    float y_pos = 0.0;
    glm::vec3 m_position = glm::vec3(0.f);
    glm::vec3 m_rotation = glm::vec3(0.f);
    glm::vec3 m_scale = glm::vec3(1.0);
    
    glm::mat4 m_viewMatrix = glm::mat4(1.f); 
    glm::mat4 m_projectionMatrix  = glm::mat4(1.f);
    glm::mat4 m_ModelMatrix = glm::mat4(1.f);
    int m_currentDemo;
    

    public:
        flib::ParticleSet<float> m_pSet;

        ParticleSimulation(size_t num, std::string vertex_shader, std::string fragment_shader);
        float random_between(float min, float max) {
            return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
        }
        size_t getParticleCount() const { return m_pSet._particles.size(); }
        int getCurrentDemo() const { return m_currentDemo; }
        //methods for vector  oflambdas:
        void  loadDemo(int index);

        void init();
        void simulation();
        //methods from the Renderable class
        void draw() override;
        Shader& getShader() override;
        void updateTime(float deltaTime) override;  
        void setViewMatrix(const glm::mat4 &viewMatrix) override;
        glm::mat4 getViewMatrix() override;
        void updateModelMatrix() override;
        glm::mat4 getModelMatrix() override;
        


};





#endif // _PARTICLE_SIMULATION_H_


