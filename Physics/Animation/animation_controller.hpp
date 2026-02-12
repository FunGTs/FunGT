#if !defined(_ANIMATION_CONTROLLER_H_)
#define _ANIMATION_CONTROLLER_H_

#include "KeyFrameRecorder/key_frame_recorder.hpp"
#include "SceneManager/scene_manager.hpp"
#include "SimpleModel/simple_model.hpp"
#include "SimpleGeometry/simple_geometry.hpp"
#include <memory>
#include <string>
#include <set>

namespace fungt {

    /**
     * AnimationController - Connects KeyframeRecorder to SceneManager
     *
     * THIS IS THE BRIDGE! It uses KeyframeRecorder to update scene objects.
     */
    class AnimationController {
    private:
        std::shared_ptr<KeyframeRecorder> m_recorder;     // Stores keyframes
        std::shared_ptr<SceneManager> m_sceneManager;     // Scene to animate

        int m_currentFrame;
        int m_maxFrame;
        bool m_isPlaying;
        float m_playbackSpeed;

        std::set<std::string> m_bakingEnabledObjects;  // Objects to auto-record

    public:
        AnimationController(std::shared_ptr<SceneManager> sceneManager)
            : m_sceneManager(sceneManager)
            , m_currentFrame(0)
            , m_maxFrame(300)
            , m_isPlaying(false)
            , m_playbackSpeed(1.0f)
        {
            m_recorder = std::make_shared<KeyframeRecorder>();
        }

        // Playback
        void play() { m_isPlaying = true; }
        void pause() { m_isPlaying = false; }
        void stop() {
            m_isPlaying = false;
            m_currentFrame = 0;
            updateFrame(0);
        }
        bool isPlaying() const { return m_isPlaying; }

        // Frame control
        void setFrame(int frame) { m_currentFrame = std::clamp(frame, 0, m_maxFrame); }
        int getCurrentFrame() const { return m_currentFrame; }
        int getMaxFrame() const { return m_maxFrame; }
        void setMaxFrame(int maxFrame) { m_maxFrame = maxFrame; }

        // Playback speed
        void setPlaybackSpeed(float speed) { m_playbackSpeed = speed; }
        float getPlaybackSpeed() const { return m_playbackSpeed; }

        // Update in render loop
        void update(float deltaTime) {
            if (m_isPlaying) {
                float frameAdvance = (deltaTime * 30.0f) * m_playbackSpeed;
                m_currentFrame += static_cast<int>(frameAdvance);

                if (m_currentFrame >= m_maxFrame) {
                    m_currentFrame = 0;  // Loop
                }

                updateFrame(m_currentFrame);
            }
        }

        // ========== KEYFRAME RECORDING ==========

        void recordKeyframe(const std::string& objectID) {
            auto object = findObject(objectID);
            if (!object) {
                std::cout << "ERROR: Object '" << objectID << "' not found!" << std::endl;
                return;
            }

            fungt::Vec3 pos, rot, scl;
            getObjectTransform(object, pos, rot, scl);

            m_recorder->recordKeyframe(objectID, m_currentFrame, pos, rot, scl);
        }

        void deleteKeyframe(const std::string& objectID, int frame) {
            m_recorder->deleteKeyframe(objectID, frame);
        }

        void clearTrack(const std::string& objectID) {
            m_recorder->clearTrack(objectID);
        }

        // ========== PHYSICS BAKING ==========

        /**
         * Enable/disable auto-recording for specific object
         */
        void setObjectBakingEnabled(const std::string& objectID, bool enabled) {
            if (enabled) {
                m_bakingEnabledObjects.insert(objectID);
            }
            else {
                m_bakingEnabledObjects.erase(objectID);
            }
        }

        bool isObjectBakingEnabled(const std::string& objectID) const {
            return m_bakingEnabledObjects.find(objectID) != m_bakingEnabledObjects.end();
        }

        /**
         * Record physics frame for ALL objects that have baking enabled
         * Call this every frame when physics is running
         */
        void recordPhysicsFrame(int frame) {
            const auto& renderables = m_sceneManager->getRenderable();

            for (const auto& obj : renderables) {
                auto model = std::dynamic_pointer_cast<SimpleModel>(obj);
                if (!model) continue;

                std::string id = model->getAnimationID();

                // Skip if baking not enabled for this object
                if (m_bakingEnabledObjects.find(id) == m_bakingEnabledObjects.end()) {
                    continue;
                }

                // Get physics body
                auto rigidBodyWeak = model->getRigidBody();
                auto rigidBody = rigidBodyWeak.lock();

                if (!rigidBody) continue;  // No physics

                // Record position from RigidBody
                fungt::Vec3 pos = rigidBody->m_pos;
                fungt::Vec3 rot(0, 0, 0);  // TODO: Convert quaternion to euler
                fungt::Vec3 scl = model->getScale();

                m_recorder->recordKeyframe(id, frame, pos, rot, scl);
            }
        }

        // ========== THE KEY METHOD! ==========

        /**
         * updateFrame() - Apply keyframes to ALL scene objects
         *
         * THIS IS WHERE KEYFRAMERECORDER IS USED!
         *
         * For each object:
         *   1. Check if has keyframes
         *   2. Get interpolated transform from KeyframeRecorder
         *   3. Apply to object (position/rotation/scale)
         *   4. Update model matrix
         */
        void updateFrame(int frame) {
            if (!m_sceneManager) return;

            const auto& renderables = m_sceneManager->getRenderable();

            for (const auto& obj : renderables) {
                // SimpleModel
                auto model = std::dynamic_pointer_cast<SimpleModel>(obj);
                if (model) {
                    std::string id = model->getAnimationID();
                    if (m_recorder->hasKeyframes(id)) {
                        applyKeyframeToModel(model, id, frame);
                    }
                    continue;
                }

                // SimpleGeometry
                auto geom = std::dynamic_pointer_cast<SimpleGeometry>(obj);
                if (geom) {
                    std::string id = geom->getAnimationID();
                    if (m_recorder->hasKeyframes(id)) {
                        applyKeyframeToGeometry(geom, id, frame);
                    }
                }
            }
        }

        std::shared_ptr<KeyframeRecorder> getRecorder() { return m_recorder; }

    private:
        std::shared_ptr<Renderable> findObject(const std::string& objectID) {
            const auto& renderables = m_sceneManager->getRenderable();

            for (const auto& obj : renderables) {
                auto model = std::dynamic_pointer_cast<SimpleModel>(obj);
                if (model && model->getAnimationID() == objectID) {
                    return model;
                }

                auto geom = std::dynamic_pointer_cast<SimpleGeometry>(obj);
                if (geom && geom->getAnimationID() == objectID) {
                    return geom;
                }
            }
            return nullptr;
        }

        void getObjectTransform(std::shared_ptr<Renderable> obj,
            fungt::Vec3& pos, fungt::Vec3& rot, fungt::Vec3& scl) {
            auto model = std::dynamic_pointer_cast<SimpleModel>(obj);
            if (model) {
                pos = model->getPosition();
                rot = model->getRotation();
                scl = model->getScale();
                return;
            }

            auto geom = std::dynamic_pointer_cast<SimpleGeometry>(obj);
            if (geom) {
                pos = geom->getPosition();
                rot = geom->getRotation();
                scl = geom->getScale();
            }
        }

        /**
         * THIS IS THE MAGIC!
         * Gets interpolated transform and applies it to object
         */
        void applyKeyframeToModel(std::shared_ptr<SimpleModel> model,
            const std::string& id, int frame) {
            fungt::Vec3 pos, rot, scl;

            // GET FROM KEYFRAMERECORDER!
            if (m_recorder->getInterpolatedTransform(id, frame, pos, rot, scl)) {
                // APPLY TO MODEL!
                model->position(pos.x, pos.y, pos.z);
                model->rotation(rot.x, rot.y, rot.z);
                model->scale(scl.x, scl.y, scl.z);
                model->updateModelMatrix();
            }
        }

        void applyKeyframeToGeometry(std::shared_ptr<SimpleGeometry> geom,
            const std::string& id, int frame) {
            fungt::Vec3 pos, rot, scl;

            // GET FROM KEYFRAMERECORDER!
            if (m_recorder->getInterpolatedTransform(id, frame, pos, rot, scl)) {
                // APPLY TO GEOMETRY!
                geom->position(pos.x, pos.y, pos.z);
                geom->rotation(rot.x, rot.y, rot.z);
                geom->scale(scl.x, scl.y, scl.z);
                geom->updateModelMatrix();
            }
        }
    };

} // namespace fungt

#endif // _ANIMATION_CONTROLLER_H_