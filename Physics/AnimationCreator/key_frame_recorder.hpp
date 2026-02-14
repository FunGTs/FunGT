#if !defined(_KEYFRAME_RECORDER_H_)
#define _KEYFRAME_RECORDER_H_

#include <map>
#include <string>
#include <memory>
#include <iostream>
#include <algorithm>
#include "Vector/vector3.hpp"

namespace fungt {

    // Single keyframe data
    struct Keyframe {
        int frame;
        fungt::Vec3 position;
        fungt::Vec3 rotation;  // Euler angles in degrees
        fungt::Vec3 scale;

        Keyframe()
            : frame(0)
            , position(0, 0, 0)
            , rotation(0, 0, 0)
            , scale(1, 1, 1)
        {
        }

        Keyframe(int f, fungt::Vec3 pos, fungt::Vec3 rot, fungt::Vec3 scl)
            : frame(f)
            , position(pos)
            , rotation(rot)
            , scale(scl)
        {
        }
    };

    // Manages keyframes for all animated objects
    class KeyframeRecorder {
    private:
        // objectID → (frame → keyframe)
        std::map<std::string, std::map<int, Keyframe>> m_tracks;

    public:
        KeyframeRecorder() = default;
        ~KeyframeRecorder() = default;

        // Record a keyframe for an object
        void recordKeyframe(const std::string& objectID, int frame,
            const fungt::Vec3& pos, const fungt::Vec3& rot, const fungt::Vec3& scl);

        // Delete a keyframe
        void deleteKeyframe(const std::string& objectID, int frame);

        // Delete all keyframes for an object
        void clearTrack(const std::string& objectID);

        // Clear all keyframes
        void clearAll();

        // Get interpolated transform at any frame (including between keyframes)
        bool getInterpolatedTransform(const std::string& objectID, float frame,
            fungt::Vec3& outPos, fungt::Vec3& outRot, fungt::Vec3& outScl) const;

        // Check if object has keyframes
        bool hasKeyframes(const std::string& objectID) const;

        // Get keyframe count for object
        int getKeyframeCount(const std::string& objectID) const;

        // Get all keyframe frame numbers for an object
        std::vector<int> getKeyframeNumbers(const std::string& objectID) const;

        // Check if specific frame has a keyframe
        bool hasKeyframeAt(const std::string& objectID, int frame) const;

        // Get exact keyframe (returns nullptr if doesn't exist)
        const Keyframe* getKeyframe(const std::string& objectID, int frame) const;

    private:
        // Linear interpolation helper
        fungt::Vec3 lerp(const fungt::Vec3& a, const fungt::Vec3& b, float t) const;
    };

} // namespace fungt

#endif // _KEYFRAME_RECORDER_H_