#include "key_frame_recorder.hpp"


namespace fungt {

    void KeyframeRecorder::recordKeyframe(const std::string& objectID, int frame,
        const fungt::Vec3& pos, const fungt::Vec3& rot, const fungt::Vec3& scl) {
        Keyframe kf(frame, pos, rot, scl);
        m_tracks[objectID][frame] = kf;

        std::cout << "Recorded keyframe for '" << objectID << "' at frame " << frame << std::endl;
    }

    void KeyframeRecorder::deleteKeyframe(const std::string& objectID, int frame) {
        auto trackIt = m_tracks.find(objectID);
        if (trackIt != m_tracks.end()) {
            trackIt->second.erase(frame);
            std::cout << "Deleted keyframe for '" << objectID << "' at frame " << frame << std::endl;
        }
    }

    void KeyframeRecorder::clearTrack(const std::string& objectID) {
        m_tracks.erase(objectID);
        std::cout << "Cleared all keyframes for '" << objectID << "'" << std::endl;
    }

    void KeyframeRecorder::clearAll() {
        m_tracks.clear();
        std::cout << "Cleared all keyframes" << std::endl;
    }

    bool KeyframeRecorder::getInterpolatedTransform(const std::string& objectID, float frame,
        fungt::Vec3& outPos, fungt::Vec3& outRot, fungt::Vec3& outScl) const {
        auto trackIt = m_tracks.find(objectID);
        if (trackIt == m_tracks.end() || trackIt->second.empty()) {
            return false;  // No keyframes for this object
        }

        const auto& keyframes = trackIt->second;

        // Find surrounding keyframes
        auto upper = keyframes.lower_bound((int)frame);

        // Exact match
        if (upper != keyframes.end() && upper->first == (int)frame) {
            outPos = upper->second.position;
            outRot = upper->second.rotation;
            outScl = upper->second.scale;
            return true;
        }

        // Before first keyframe - use first keyframe
        if (upper == keyframes.begin()) {
            outPos = upper->second.position;
            outRot = upper->second.rotation;
            outScl = upper->second.scale;
            return true;
        }

        // After last keyframe - use last keyframe
        if (upper == keyframes.end()) {
            auto last = std::prev(upper);
            outPos = last->second.position;
            outRot = last->second.rotation;
            outScl = last->second.scale;
            return true;
        }

        // Between two keyframes - interpolate
        auto lower = std::prev(upper);

        int frame1 = lower->first;
        int frame2 = upper->first;
        float t = (frame - frame1) / (float)(frame2 - frame1);  // 0.0 to 1.0

        outPos = lerp(lower->second.position, upper->second.position, t);
        outRot = lerp(lower->second.rotation, upper->second.rotation, t);
        outScl = lerp(lower->second.scale, upper->second.scale, t);

        return true;
    }

    bool KeyframeRecorder::hasKeyframes(const std::string& objectID) const {
        auto it = m_tracks.find(objectID);
        return it != m_tracks.end() && !it->second.empty();
    }

    int KeyframeRecorder::getKeyframeCount(const std::string& objectID) const {
        auto it = m_tracks.find(objectID);
        return (it != m_tracks.end()) ? it->second.size() : 0;
    }

    std::vector<int> KeyframeRecorder::getKeyframeNumbers(const std::string& objectID) const {
        std::vector<int> frames;
        auto it = m_tracks.find(objectID);
        if (it != m_tracks.end()) {
            for (const auto& kv : it->second) {
                frames.push_back(kv.first);
            }
        }
        return frames;
    }

    bool KeyframeRecorder::hasKeyframeAt(const std::string& objectID, int frame) const {
        auto trackIt = m_tracks.find(objectID);
        if (trackIt == m_tracks.end()) return false;

        return trackIt->second.find(frame) != trackIt->second.end();
    }

    const Keyframe* KeyframeRecorder::getKeyframe(const std::string& objectID, int frame) const {
        auto trackIt = m_tracks.find(objectID);
        if (trackIt == m_tracks.end()) return nullptr;

        auto kfIt = trackIt->second.find(frame);
        if (kfIt == trackIt->second.end()) return nullptr;

        return &kfIt->second;
    }

    fungt::Vec3 KeyframeRecorder::lerp(const fungt::Vec3& a, const fungt::Vec3& b, float t) const {
        return fungt::Vec3(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t
        );
    }

} // namespace fungt