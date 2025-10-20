#if !defined(_VECTOR3_HPP_)
#define _VECTOR3_HPP_
#include <cmath>
#include "../include/glmath.hpp"
namespace fungt{


    class  Vec3 {

        public:
            float x, y, z;
     
            Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
            
            Vec3 operator+(const Vec3& other) const {
                return Vec3(x + other.x, y + other.y, z + other.z);
            }
            
            Vec3 operator-(const Vec3& other) const {
                return Vec3(x - other.x, y - other.y, z - other.z);
            }
            
            Vec3 operator*(float scalar) const {
                return Vec3(x * scalar, y * scalar, z * scalar);
            }
            Vec3 operator/(float scalar) const {
                return Vec3(x / scalar, y / scalar, z / scalar);
            }
            Vec3& operator+=(const Vec3& other) {
                x += other.x; y += other.y; z += other.z;
                return *this;
            }
            Vec3& operator-=(const Vec3& other) {
                x -= other.x; y -= other.y; z -= other.z;
                return *this;
            }
            float dot(const Vec3& other) const {
                return x * other.x + y * other.y + z * other.z;
            }
            
            Vec3 cross(const Vec3& other) const {
                return Vec3(
                    y * other.z - z * other.y,
                    z * other.x - x * other.z,
                    x * other.y - y * other.x
                );
            }
            
            float length() const {
                return std::sqrt(x * x + y * y + z * z);
            }
            
            Vec3 normalize() const {
                float len = length();
                if (len > 0) return Vec3(x/len, y/len, z/len);
                return Vec3(0, 0, 0);
            }
    };
    inline fungt::Vec3 operator*(float scalar, const fungt::Vec3& v) {
        return fungt::Vec3(v.x * scalar, v.y * scalar, v.z * scalar);
    }
    inline Vec3 toFungtVec3(const glm::vec3& v) {
        return Vec3(v.x, v.y, v.z);
    }

}






#endif // _VECTOR3_HPP_)
