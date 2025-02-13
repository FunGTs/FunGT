#if !defined(_PARTICLE_SYS_H_)
#define _PARTICLE_SYS_H_
#include "particle.hpp"
#include "../../Renderable/renderable.hpp"
#include "../../VertexGL/vertexArrayObjects.hpp"
#include "../../VertexGL/vertexBuffers.hpp"
#include "../../VertexGL/vertexIndices.hpp"
#include "../../Shaders/shader.hpp"
class ParticleSystem : public Renderable { //

    
    VertexArrayObject m_vao; 
    VertexBuffer m_vbo; 
    Shader m_shader;
    size_t m_NumParticles; 
    float m_deltaTime = 2.0;
    float x_pos = 0.0;
    float y_pos = 0.0;

    
    
    glm::mat4 m_viewMatrix = glm::mat4(1.f); 
    glm::mat4 m_projectionMatrix  = glm::mat4(1.f);

    protected:
        std::array<float, 3> m_startPosition = {0.0f,0.f,0.0f};

    public:
        std::vector<Particle> m_particles; 
    
    public: 
        ParticleSystem(size_t num, std::string vs, std::string fs);
        void init();
        void update(); 
        virtual void updateParticles();
        virtual void applyForce(int index_particle);
        virtual void resetParticle(int index_particle);
        virtual void moveParticle(int index_particle);
        void draw() override;
        Shader& getShader() override;
        void updateTime(float deltaTime) override;  
        void setViewMatrix(const glm::mat4 &viewMatrix) override;
        glm::mat4 getViewMatrix() override;
    


};

#endif // _PARTICLE_SYS_H_
