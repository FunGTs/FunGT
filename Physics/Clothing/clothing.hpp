#if !defined(_CLOTHING_H_)
#define _CLOTHING_H_
#include "../../Renderable/renderable.hpp"
#include "../../VertexGL/vertexArrayObjects.hpp"
#include "../../VertexGL/vertexBuffers.hpp"
#include "../../VertexGL/vertexIndices.hpp"
#include "../../Shaders/shader.hpp"
#include "../../Vector/vector3.hpp"
#include "../../Path_Manager/path_manager.hpp"
#include <funlib/funlib.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>
#include <array>
class Clothing : public Renderable
{

    //OpenGL render
    VertexArrayObject m_vao; 
    std::array<VertexBuffer,2> m_vbo;
    int currentBuffer = 0;  // Which buffer is currently active
    VertexIndex m_vi; 
    Shader m_shader;

    float m_deltaTime = 2.0;
    //Clothing stuff:
    int   grid_x = 0;
    int   grid_y = 0;
    std::vector<unsigned int> indexData;
    int numIndices;
    const GLuint PRIM_RESTART = 0xffffffff;
    std::vector<fungt::Vec3> posIn;
    std::vector<fungt::Vec3> velIn;
    std::vector<fungt::Vec3> velOut;
    // Simulation parameters (tweak to taste)
    float RestLengthHoriz; // normalized spacing
    float RestLengthVert;
    float RestLengthDiag;
    const float GravityY = -9.81f;
    const float ParticleMass = 0.1f;
    const float ParticleInvMass = 1.0f / ParticleMass;
    const float DeltaT = 0.0005f;      // Larger timestep  
    const int StepsPerFrame = 10;      // Fewer steps
    const float SpringK = 20000.0f;
    const float DampingConst = 0.1f;

    std::size_t GridSize = 0;
    //Renderable object stuff
    glm::mat4 m_viewMatrix = glm::mat4(1.f); 
    glm::mat4 m_projectionMatrix  = glm::mat4(1.f);
    glm::mat4 m_ModelMatrix = glm::mat4(1.f);
    glm::vec3 m_position = glm::vec3(0.f);
    glm::vec3 m_rotation = glm::vec3(0.f);
    glm::vec3 m_scale = glm::vec3(1.0);
    void initIndices();
    public:
        
        Clothing(int nx, int ny);

        

        void init();
        void simulation();
        //methods from the Renderable class
        void draw() override;
        Shader& getShader() override;
        void updateTime(float deltaTime) override;  
        void setViewMatrix(const glm::mat4 &viewMatrix) override;
        glm::mat4 getViewMatrix() override;
        void updateModelMatrix() override;
        glm::mat4 getModelMatrix() const override;



};






#endif // _CLOTHING_H_
