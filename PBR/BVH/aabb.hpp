#if !defined(_AABB_H_)
#define _AABB_H_
#include "Vector/vector3.hpp"
#include "gpu/include/fgt_cpu_device.hpp"
#include <cmath>
class AABB {

public:    
    fungt::Vec3 m_min;
    fungt::Vec3 m_max;

    fgt_device AABB(){
        m_min = fungt::Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
        m_max = fungt::Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    }

    fgt_device AABB(const fungt::Vec3& mn, const fungt::Vec3& mx) {
        m_min = mn;
        m_max = mx;
    }
    fgt_device void grow(const fungt::Vec3& p) {
        m_min.x =std::fminf(m_min.x, p.x);
        m_min.y =std::fminf(m_min.y, p.y);
        m_min.z =std::fminf(m_min.z, p.z);
        m_max.x =std::fmaxf(m_max.x, p.x);
        m_max.y =std::fmaxf(m_max.y, p.y);
        m_max.z =std::fmaxf(m_max.z, p.z);
    }
    // Expand box to include another box
    fgt_device void grow(const AABB& other) {
        grow(other.m_min);
        grow(other.m_max);
    }
    fgt_device fungt::Vec3 center() const {
        fungt::Vec3 mid = m_min + m_max; 
        mid = mid/0.5;
        return mid; 
    }
    // Surface area (used for building quality BVH)
    fgt_device float surfaceArea() const {
        fungt::Vec3 d = m_max - m_min;
        return 2.0f * (d.x * d.y + d.y * d.z + d.z * d.x);
    }

};

#endif // _AABB_H_
