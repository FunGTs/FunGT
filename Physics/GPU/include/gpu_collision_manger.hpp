#if !defined(_GPU_COLLISION_MANAGER_HPP_)
#define _GPU_COLLISION_MANAGER_HPP_

#include "gpu_physics_kernel.hpp"
#include <vector>
#include <memory>

namespace gpu {

    struct BodyGroup {
        int startIndex;
        int count;
    };

    class CollisionManager {
    private:
        std::shared_ptr<PhysicsKernel> m_kernel;
        std::vector<BodyGroup> m_groups;

        int m_currentGroupStart;
        bool m_isGroupOpen;

    public:
        CollisionManager(std::shared_ptr<PhysicsKernel> kernel)
            : m_kernel(kernel), m_currentGroupStart(0), m_isGroupOpen(false) {
        }

        // Start a new group
        void beginGroup() {
            if (m_isGroupOpen) {
                throw std::runtime_error("CollisionManager::beginGroup() - Group already open! Call endGroup() first.");
            }
            m_currentGroupStart = m_kernel->getNumBodies();
            m_isGroupOpen = true;
        }

        // End current group
        void endGroup() {
            if (!m_isGroupOpen) {
                throw std::runtime_error("CollisionManager::endGroup() - No group open! Call beginGroup() first.");
            }

            int count = m_kernel->getNumBodies() - m_currentGroupStart;
            m_groups.push_back({ m_currentGroupStart, count });
            m_isGroupOpen = false;
        }

        // Add sphere  //Mode dynamic by default
        int addSphere(float x, float y, float z, float radius, float mass, MODE mode = MODE::DYNAMIC) {
            if (!m_isGroupOpen) {
                throw std::runtime_error("CollisionManager::addSphere() - No group open! Call beginGroup() first.");
            }
            return m_kernel->addSphere(x, y, z, radius, mass, mode);
        }

        // Add box  //Mode dynamic by default
        int addBox(float x, float y, float z, float width, float height, float depth, float mass, MODE mode = MODE::DYNAMIC) {
            if (!m_isGroupOpen) {
                throw std::runtime_error("CollisionManager::addBox() - No group open! Call beginGroup() first.");
            }
            return m_kernel->addBox(x, y, z, width, height, depth, mass,mode);
        }

        // Get number of groups
        int getNumGroups() const {
            return m_groups.size();
        }

        // Get a specific group
        BodyGroup getGroup(int index) const {
            return m_groups[index];
        }

        // Get number of bodies
        int getNumBodies() const {
            return m_kernel->getNumBodies();
        }

        // Get kernel
        std::shared_ptr<PhysicsKernel> getKernel() {
            return m_kernel;
        }
    };

} // namespace gpu

#endif