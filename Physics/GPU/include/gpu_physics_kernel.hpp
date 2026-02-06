#if !defined(_GPU_PHYSICS_KERNEL_HPP_)
#define _GPU_PHYSICS_KERNEL_HPP_

#include <GL/glew.h>
#include <funlib/funlib.hpp>
#include <CL/opencl.h>
#include "gpu_device_data.hpp"
#include "gpu_memory_utils.hpp"
#include "gpu_manifold_contacts.hpp"
#include "gpu_manifold_utils.hpp"
#include "gpu_collision_detection.hpp"
#include "gpu_impulse_solver.hpp"
enum class MODE {
    STATIC,
    DYNAMIC
};
namespace gpu {

    class PhysicsKernel {
    private:
        sycl::queue m_queue;

        // Physics state (Structure of Arrays)
        DeviceData m_data;

        // Rendering (SYCL-OpenGL interop)
        unsigned int m_modelMatrixSSBO;

        // Manifold cache
        GPUManifold* m_manifolds;
        int* m_numManifolds;          // GPU counter
        int m_maxManifolds;

        // Pair hash table (pair key -> manifold index, -1 if empty)
        int* m_pairToManifold;
        int m_hashTableSize;          // should be prime, larger than maxManifolds
                

        // Metadata
        int m_numBodies;
        int m_capacity;
        float m_worldSize = 200.f;
        float m_cellSize = 5.0f;

    public:
        PhysicsKernel();
        ~PhysicsKernel();

        // Initialization
        void init(int maxBodies);
        void cleanup();

        // Body management
        int addSphere(float x, float y, float z, float radius, float mass, MODE mode); //Mode dynamic by default
        int addBox(float x, float y, float z, float width, float height, float depth, float mass, MODE mode);

        // Physics pipeline (no parameters needed - uses member data!)
        void applyForces(float dt);
        void integrate(float dt);
        void broadPhase();          // TODO: spatial hashing
        void narrowPhase();         // TODO: collision detection
        void solveImpulses(float dt);               // TODO: impulse solver
        void buildMatrices();       // Compute model matrices for rendering

        // Getters
        int getNumBodies() const { return m_numBodies; }
        unsigned int getModelMatrixSSBO() const { return m_modelMatrixSSBO; }
        void clearManiFolds();
        void debugManifolds();
        // Collision detection
        void detectStaticVsDynamic();   // spheres vs ground
        //void detectDynamicVsDynamic();  // spheres vs spheres (later, with grid)
        void debugVelocity(int bodyId);
        //void refreshManifolds();        // update world positions from local
        //void pruneOldContacts();        // remove contacts that separated
    };

} // namespace gpu

#endif // _GPU_PHYSICS_KERNEL_HPP_ 