#if !defined(_QUATERNION_H_)
#define _QUATERNION_H_ 
#include "../Vector/vector3.hpp"
#include "../Matrix/matrix3x3f.hpp"
// Quaternion class for stable 3D rotations
struct Quaternion {
    float w, x, y, z;
    
    Quaternion(float w = 1, float x = 0, float y = 0, float z = 0) : w(w), x(x), y(y), z(z) {}
    
    // Create quaternion from axis-angle
    static Quaternion fromAxisAngle(const fungt::Vec3& axis, float angle) {
        float halfAngle = angle * 0.5f;
        float sinHalf = std::sin(halfAngle);
        fungt::Vec3 normalizedAxis = axis.normalize();
        
        return Quaternion(
            std::cos(halfAngle),
            normalizedAxis.x * sinHalf,
            normalizedAxis.y * sinHalf,
            normalizedAxis.z * sinHalf
        );
    }
    
    // Quaternion multiplication
    Quaternion operator*(const Quaternion& other) const {
        return Quaternion(
            w * other.w - x * other.x - y * other.y - z * other.z,
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w
        );
    }
    
    // Normalize quaternion
    Quaternion normalize() const {
        float len = std::sqrt(w*w + x*x + y*y + z*z);
        if (len > 0) {
            return Quaternion(w/len, x/len, y/len, z/len);
        }
        return Quaternion(1, 0, 0, 0);
    }
    
    // Convert to rotation matrix
    fungt::Matrix3f toMatrix() const {

        fungt::Matrix3f result;
        
        float xx = x * x, yy = y * y, zz = z * z;
        float xy = x * y, xz = x * z, yz = y * z;
        float wx = w * x, wy = w * y, wz = w * z;
        
        result.m[0][0] = 1 - 2 * (yy + zz);
        result.m[0][1] = 2 * (xy - wz);
        result.m[0][2] = 2 * (xz + wy);
        
        result.m[1][0] = 2 * (xy + wz);
        result.m[1][1] = 1 - 2 * (xx + zz);
        result.m[1][2] = 2 * (yz - wx);
        
        result.m[2][0] = 2 * (xz - wy);
        result.m[2][1] = 2 * (yz + wx);
        result.m[2][2] = 1 - 2 * (xx + yy);
        
        return result;
    }
    
    // Rotate a vector by this quaternion
    fungt::Vec3 rotateVector(const fungt::Vec3& v) const {
        // v' = q * (0, v) * q^(-1)
        Quaternion vecQuat(0, v.x, v.y, v.z);
        Quaternion conjugate(w, -x, -y, -z);
        Quaternion result = (*this) * vecQuat * conjugate;
        return fungt::Vec3(result.x, result.y, result.z);
    }
};
#endif // _QUATERNION_H_   
