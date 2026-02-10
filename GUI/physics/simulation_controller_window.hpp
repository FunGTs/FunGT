#if !defined(_SIMULATION_CONTROLLER_H_)
#define _SIMULATION_CONTROLLER_H_
#include "Physics/RigidBody/rigid_body.hpp"
#include <memory>
#include <vector>

// Forward declaration
class RigidBody;
class Quaternion;

namespace fungt {

    enum class SimulationState {
        STOPPED,
        PLAYING,
        PAUSED
    };

    // Snapshot of a single rigid body's state
    struct RigidBodySnapshot {
        Vec3 pos;
        Vec3 vel;
        Vec3 angularVel;
        Vec3 force;
        Vec3 torque;
        Quaternion orientation;
        std::shared_ptr<RigidBody> body;

        RigidBodySnapshot(std::shared_ptr<RigidBody> rb);
        void restore();
    };

    class SimulationController {
    private:
        SimulationState m_state;
        float m_currentTime;
        float m_playbackSpeed;
        std::vector<RigidBodySnapshot> m_snapshots;

    public:
        SimulationController()
            : m_state(SimulationState::STOPPED)
            , m_currentTime(0.0f)
            , m_playbackSpeed(1.0f)
        {
        }

        // State control
        void play() {
            m_state = SimulationState::PLAYING;
        }

        void pause() {
            if (m_state == SimulationState::PLAYING) {
                m_state = SimulationState::PAUSED;
            }
        }

        void stop() {
            m_state = SimulationState::STOPPED;
            m_currentTime = 0.0f;
        }

        void reset() {
            stop();
            restoreSnapshots();
        }

        // State queries
        SimulationState getState() const { return m_state; }
        bool isPlaying() const { return m_state == SimulationState::PLAYING; }
        bool isPaused() const { return m_state == SimulationState::PAUSED; }
        bool isStopped() const { return m_state == SimulationState::STOPPED; }

        // Time management
        float getCurrentTime() const { return m_currentTime; }

        void updateTime(float dt) {
            if (m_state == SimulationState::PLAYING) {
                m_currentTime += dt * m_playbackSpeed;
            }
        }

        float getPlaybackSpeed() const { return m_playbackSpeed; }
        void setPlaybackSpeed(float speed) { m_playbackSpeed = speed; }

        // Snapshot management
        void captureSnapshot(std::shared_ptr<RigidBody> body) {
            m_snapshots.emplace_back(body);
        }

        void restoreSnapshots() {
            for (auto& snapshot : m_snapshots) {
                snapshot.restore();
            }
        }

        void clearSnapshots() {
            m_snapshots.clear();
        }
    };

} // namespace fungt

#endif // _SIMULATION_CONTROLLER_H_