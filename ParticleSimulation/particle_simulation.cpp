#include "particle_simulation.hpp"

ParticleSimulation::ParticleSimulation(size_t num, std::string vertex_shader, std::string fragment_shader)
: m_NumParticles{num}{
    
    m_pSet.SetNumParticles(m_NumParticles);
    std::cout<<"Particle system constructor"<<std::endl;
    std::cout<<"Num particles: "<<m_pSet._particles.size()<<std::endl;
   
    float radius = 0.5f;  // Radius from the origin
    float spread = 0.1f;   // Variability of the particle positions
    std::size_t num_particles = m_pSet._particles.size();
    
    // Distribute particles randomly within a radius from the origin (0, 0, 0)
    for (std::size_t i = 0; i < num_particles; ++i) {
        // Random angle between 0 and 2π
        float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;
        // Random distance from the center within the specified radius
        float distance = static_cast<float>(rand()) / RAND_MAX * radius;
    
        // Initial position within a circular region
        m_pSet._particles[i].position[0] = distance * std::cos(angle); // X
        m_pSet._particles[i].position[1] = distance * std::sin(angle); // Y
        m_pSet._particles[i].position[2] = 0.0f;  // Z is 0 (flat 2D)
    
        // Give a random initial velocity to the particles (optional)
        m_pSet._particles[i].velocity[0] = 0.0f;  // No initial velocity in X direction
        m_pSet._particles[i].velocity[1] = 0.0f;  // No initial velocity in Y direction
        m_pSet._particles[i].velocity[2] = 0.0f;  // No initial velocity in Z direction
    }
    
    //Print just the position of the firs 2 particles
    std::cout << "Particle positions:" << std::endl;
    for (std::size_t i = 0; i < 2; ++i) {
        std::cout << "Particle "<< i << ": ("
                  << m_pSet._particles[i].position[0] << ", "
                  << m_pSet._particles[i].position[1] << ", "
                  << m_pSet._particles[i].position[2] << ")" << std::endl;
    }
    //m_pSet.print();
    this->init();
    m_shader.create(vertex_shader,fragment_shader);
}

void ParticleSimulation::init()
{
    m_vao.genVAO();
    m_vbo.genVB();

    //Bind

    m_vao.bind();

    m_vbo.bind();
    //m_vbo.bufferData(m_pSet._particles.data(),m_pSet._particles.size()*sizeof(flib::Particle<float>),GL_DYNAMIC_DRAW);
    m_vbo.bufferData(m_pSet._particles.data(),m_pSet._particles.size()*sizeof(flib::Particle<float>),GL_DYNAMIC_DRAW); 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(flib::Particle<float>), (void*)offsetof(flib::Particle<float>, position));
    glEnableVertexAttribArray(0);

    m_vao.unbind();
}
void ParticleSimulation::draw()
{
    this->simulation();
    glEnable(GL_PROGRAM_POINT_SIZE);
    m_vao.bind();
    glDrawArrays(GL_POINTS, 0, m_NumParticles);
}

Shader &ParticleSimulation::getShader()
{
    // TODO: insert return statement here
    return m_shader;
}

void ParticleSimulation::updateTime(float deltaTime)
{
    this->m_deltaTime = deltaTime; 
}

void ParticleSimulation::setViewMatrix(const glm::mat4 &viewMatrix)
{
    m_viewMatrix = viewMatrix;
}

glm::mat4 ParticleSimulation::getViewMatrix()
{
    return m_viewMatrix;
}
void ParticleSimulation::updateModelMatrix()
{
    m_ModelMatrix = glm::mat4(1.f);
    m_ModelMatrix = glm::translate(m_ModelMatrix, m_position);
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.x), glm::vec3(1.f, 0.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.y), glm::vec3(0.f, 1.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.z), glm::vec3(0.f, 0.f, 1.f));
    m_ModelMatrix = glm::scale(m_ModelMatrix, m_scale);
}
glm::mat4 ParticleSimulation::getModelMatrix()
{
    return m_ModelMatrix;
}
void ParticleSimulation::simulation()
{


    auto lambda = [](flib::Particle<float> &p, float dt) {
        const float G1 = 1000.f;
        const float G2 = 1000.f;

        float blackHolePos1[] = {5.0f, 0.0f, 0.0f};
        float blackHolePos2[] = {-5.0f, 0.0f, 0.0f};
        const float MaxDist = 45.0f;
        const float ParticleMass = 0.1f;
        const float InvMass = 1.0f / ParticleMass;


        const float DeltaT = 0.0005;

        float d1[] = {
            blackHolePos1[0] - p.position[0],
            blackHolePos1[1] - p.position[1],
            blackHolePos1[2] - p.position[2]
        };
        float gdist1 = sycl::sqrt(
            sycl::pow(d1[0], 2) + sycl::pow(d1[1], 2) + sycl::pow(d1[2], 2)
        );
        float force[] = {
            (G1 / gdist1) * (d1[0] / gdist1),
            (G1 / gdist1) * (d1[1] / gdist1),
            (G1 / gdist1) * (d1[2] / gdist1)
        };

        // Distance vector to Black Hole 2
        float d2[] = {
            blackHolePos2[0] - p.position[0],
            blackHolePos2[1] - p.position[1],
            blackHolePos2[2] - p.position[2]
        };
        float gdist2 =  sycl::sqrt(
            sycl::pow(d2[0], 2) + sycl::pow(d2[1], 2) + sycl::pow(d2[2], 2)
        );

        force[0] += (G2 / gdist2) * (d2[0] / gdist2);
        force[1] += (G2 / gdist2) * (d2[1] / gdist2);
        force[2] += (G2 / gdist2) * (d2[2] / gdist2);
        if (gdist1 > MaxDist) {
            p.position[0] = 0.0f;
            p.position[1] = 0.0f;
            p.position[2] = 0.0f;

            p.velocity[0] = 0.0f;
            p.velocity[1] = 0.0f;
            p.velocity[2] = 0.0f;
        } else {
            float a[] = {
                force[0] * InvMass,
                force[1] * InvMass,
                force[2] * InvMass
            };

            p.position[0] += p.velocity[0] * DeltaT + 0.5f * a[0] * DeltaT * DeltaT;
            p.position[1] += p.velocity[1] * DeltaT + 0.5f * a[1] * DeltaT * DeltaT;
            p.position[2] += p.velocity[2] * DeltaT + 0.5f * a[2] * DeltaT * DeltaT;

            p.velocity[0] += a[0] * DeltaT;
            p.velocity[1] += a[1] * DeltaT;
            p.velocity[2] += a[2] * DeltaT;
        }
    };
    auto myFunc = [](flib::Particle<float> &p, float dt) {
        constexpr float gravity = -9.81f;

        // Apply acceleration to velocity
        p.velocity[1] += gravity * dt;
    
        // Update position with Euler integration
        p.position[0] += p.velocity[0] * dt;
        p.position[1] += p.velocity[1] * dt;
        p.position[2] += p.velocity[2] * dt;
        if (p.position[1] < -5.0f) { // Fell too far below
            p.position[1] = 10.0f;    // Respawn at top
            p.velocity[1] = 0.0f;
        }
    };
    auto spiralExplosionSim = [](flib::Particle<float>& p, float dt) {
        constexpr float central_force_strength = 0.5f; // Strength of the radial force
        constexpr float spiral_speed = 0.5f;          // Speed of the spiral motion
    
        // Calculate the distance from the center (0, 0, 0)
        float distance = std::sqrt(p.position[0] * p.position[0] + p.position[1] * p.position[1]);
    
        // Apply radial force pushing particles outward
        float radial_force = central_force_strength / (distance + 0.1f); // Prevent division by zero
    
        // Update velocity to spiral outward based on distance
        p.velocity[0] += radial_force * p.position[0] * dt; // X component
        p.velocity[1] += radial_force * p.position[1] * dt; // Y component
    
        // Add a tangential velocity to make the particle spiral out
        p.velocity[0] -= spiral_speed * p.position[1] * dt; // X component
        p.velocity[1] += spiral_speed * p.position[0] * dt; // Y component
    
        // Update position based on the velocity (Euler integration)
        p.position[0] += p.velocity[0] * dt;
        p.position[1] += p.velocity[1] * dt;
        p.position[2] += p.velocity[2] * dt; // Z remains unaffected
    };
    auto vortexSim = [](flib::Particle<float>& p, float dt) {
        constexpr float vortex_strength = 2.0f;
        constexpr float lift_force = 0.3f;
        constexpr float damping = 0.98f;

        float r = std::sqrt(p.position[0] * p.position[0] + p.position[1] * p.position[1]);
        float tangential = vortex_strength / (r + 0.1f);

        // Swirl around Z-axis
        p.velocity[0] += -tangential * p.position[1] * dt;
        p.velocity[1] += tangential * p.position[0] * dt;

        // Pull inward + lift upward
        p.velocity[0] -= 0.5f * p.position[0] * dt;
        p.velocity[1] -= 0.5f * p.position[1] * dt;
        p.velocity[2] += lift_force * dt;

        p.velocity[0] *= damping;
        p.velocity[1] *= damping;

        p.position[0] += p.velocity[0] * dt;
        p.position[1] += p.velocity[1] * dt;
        p.position[2] += p.velocity[2] * dt;
    };
    auto blackHoleSim = [](flib::Particle<float>& p, float dt) {
        constexpr float gravity = 5.0f;
        constexpr float event_horizon = 0.1f;

        float dx = -p.position[0];
        float dy = -p.position[1];
        float dz = -p.position[2];
        float dist_sq = dx * dx + dy * dy + dz * dz;
        float dist = std::sqrt(dist_sq);

        if (dist > event_horizon) {
            float force = gravity / dist_sq;
            p.velocity[0] += force * (dx / dist) * dt;
            p.velocity[1] += force * (dy / dist) * dt;
            p.velocity[2] += force * (dz / dist) * dt;
        }
        else {
            // Trapped! Slow down
            p.velocity[0] *= 0.9f;
            p.velocity[1] *= 0.9f;
            p.velocity[2] *= 0.9f;
        }

        p.position[0] += p.velocity[0] * dt;
        p.position[1] += p.velocity[1] * dt;
        p.position[2] += p.velocity[2] * dt;
        };
    auto fireworkSim = [](flib::Particle<float>& p, float dt) {
        constexpr float gravity = -2.0f;
        constexpr float drag = 0.99f;

        // Gravity pulls down
        p.velocity[2] += gravity * dt;

        // Air resistance
        p.velocity[0] *= drag;
        p.velocity[1] *= drag;
        p.velocity[2] *= drag;

        p.position[0] += p.velocity[0] * dt;
        p.position[1] += p.velocity[1] * dt;
        p.position[2] += p.velocity[2] * dt;

        // Fade/shrink over time (if you have a size/alpha parameter)
        };
    auto waveSim = [](flib::Particle<float>& p, float dt) {
        float time = 0.0f;
        time += dt;

        constexpr float wave_speed = 2.0f;
        constexpr float amplitude = 0.5f;

        float wave = std::sin(p.position[0] * wave_speed + time) *
            std::cos(p.position[1] * wave_speed + time);

        p.velocity[2] = wave * amplitude;

        p.position[2] += p.velocity[2] * dt;
        };
    //SYCL update
    //std::cout<<"Update particles"<<std::endl;
    //std::cout<<"VBO ID: "<<m_vbo.getId()<<std::endl;
    int numParticles = m_pSet._particles.size();
    flib::ParticleSystem<float, decltype(waveSim)>::update_ocl(numParticles, m_vbo.getId(), waveSim, 0.005f);
  
    /*for (std::size_t i = 0; i < 2; ++i) {
        std::cout << "Particle "<< i << ": ("
                  << m_pSet._particles[i].position[0] << ", "
                  << m_pSet._particles[i].position[1] << ", "
                  << m_pSet._particles[i].position[2] << ")" << std::endl;
    }*/
    
    //std::cout<<"uopdate particles"<<std::endl;
    //m_pSet.print();
    //m_vbo.bind();
    //m_vbo.bufferSubData(m_pSet._particles.data(),m_pSet._particles.size()*sizeof(flib::Particle<float>));
}
