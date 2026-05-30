#if !defined(_VECTOR3_GLM_HPP_)
#define _VECTOR3_GLM_HPP_
#include "vector3.hpp"
#include "../include/glmath.hpp"

namespace fungt {

    inline Vec3 toFungtVec3(const glm::vec3& v) {
        return Vec3(v.x, v.y, v.z);
    }
    inline Vec3 toFungtVec3(float vec[3]) {
        return Vec3(vec[0], vec[1], vec[2]);
    }
    inline glm::vec3 toGlmVec3(const fungt::Vec3& vec) {
        return glm::vec3(vec.x, vec.y, vec.z);
    }

}

#endif