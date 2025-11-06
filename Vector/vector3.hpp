#if !defined(_VECTOR3_HPP_)
#define _VECTOR3_HPP_
#include <cmath>
#include "../include/glmath.hpp"
#include "../gpu/include/fgt_cpu_device.hpp"

// DEBUG: Print what fgt_device expands to
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#pragma message("fgt_device expands to: " TOSTRING(fgt_device))

#if defined(__KERNEL_CUDA__)
#define FGT_SQRT(x) sqrtf(x)
#pragma message("Vec3 compiled for CUDA")
#else
#define FGT_SQRT(x) std::sqrt(x)
#pragma message("Vec3 compiled for CPU")
#endif
namespace fungt{


    class  Vec3 {

        public:
            float x, y, z;
     
            fgt_device Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
            
            fgt_device Vec3 operator+(const Vec3& other) const {
                return Vec3(x + other.x, y + other.y, z + other.z);
            }
            
            fgt_device Vec3 operator-(const Vec3& other) const {
                return Vec3(x - other.x, y - other.y, z - other.z);
            }
            
            fgt_device Vec3 operator*(float scalar) const {
                return Vec3(x * scalar, y * scalar, z * scalar);
            }
            fgt_device Vec3 operator*(const Vec3& other) const {
                return Vec3(x * other.x, y * other.y, z * other.z);
            }
            fgt_device Vec3 operator/(float scalar) const {
                return Vec3(x / scalar, y / scalar, z / scalar);
            }
            fgt_device Vec3& operator+=(const Vec3& other) {
                x += other.x; y += other.y; z += other.z;
                return *this;
            }
            fgt_device Vec3& operator-=(const Vec3& other) {
                x -= other.x; y -= other.y; z -= other.z;
                return *this;
            }
            fgt_device float dot(const Vec3& other) const {
                return x * other.x + y * other.y + z * other.z;
            }
            
            fgt_device Vec3 cross(const Vec3& other) const {
                return Vec3(
                    y * other.z - z * other.y,
                    z * other.x - x * other.z,
                    x * other.y - y * other.x
                );
            }
            
            fgt_device float length() const {
                return std::sqrt(x * x + y * y + z * z);
            }
            
            fgt_device Vec3 normalize() const {
                float len = length();
                if (len > 0) return Vec3(x/len, y/len, z/len);
                return Vec3(0, 0, 0);
            }
    };
    fgt_device inline fungt::Vec3 operator*(float scalar, const fungt::Vec3& v) {
        return fungt::Vec3(v.x * scalar, v.y * scalar, v.z * scalar);
    }
    fgt_device inline Vec3 toFungtVec3(const glm::vec3& v) {
        return Vec3(v.x, v.y, v.z);
    }
    fgt_device inline Vec3 toFungtVec3(float vec[3]) {
        return Vec3(vec[0], vec[1], vec[2]);
    }
    fgt_device inline glm::vec3 toGlmVec3(const fungt::Vec3& vec) {

        return glm::vec3(vec.x, vec.y, vec.z);
    }

}






#endif // _VECTOR3_HPP_)
