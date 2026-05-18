#if !defined(_VEC4_H_)
#define _VEC4_H_
#include "gpu/include/fgt_cpu_device.hpp"
#include "vector3.hpp"

namespace fungt{


    class Vec4 {
    public:
        float x, y, z, w;

        fgt_device Vec4(float x = 0, float y = 0, float z = 0, float w = 0) : x(x), y(y), z(z), w(w) {}
        fgt_device Vec4(const Vec3& v, float w = 0.0f) : x(v.x), y(v.y), z(v.z), w(w) {}
        fgt_device fungt::Vec3 xyz() const { return Vec3(x, y, z); }

        // Arithmetic with Vec4
        fgt_device Vec4 operator+(const Vec4& o) const { return Vec4(x + o.x, y + o.y, z + o.z, w + o.w); }
        fgt_device Vec4 operator-(const Vec4& o) const { return Vec4(x - o.x, y - o.y, z - o.z, w - o.w); }
        fgt_device Vec4 operator*(float s)        const { return Vec4(x * s, y * s, z * s, w * s); }
        fgt_device Vec4 operator/(float s)        const { return Vec4(x / s, y / s, z / s, w / s); }
        fgt_device Vec4& operator+=(const Vec4& o) { x += o.x; y += o.y; z += o.z; w += o.w; return *this; }
        fgt_device Vec4& operator-=(const Vec4& o) { x -= o.x; y -= o.y; z -= o.z; w -= o.w; return *this; }
        // Scalar multiply from left    
        fgt_device friend Vec4 operator*(float s, const Vec4& v) { return Vec4(v.x * s, v.y * s, v.z * s, v.w * s); }

        // Dot and length — w ignored for geometric ops
        fgt_device float dot(const Vec4& o) const { return x * o.x + y * o.y + z * o.z; }
        fgt_device float length() const { return FGT_SQRT(x * x + y * y + z * z); }
        fgt_device Vec4 normalize() const {
            float len = length();
            if (len > 0) return Vec4(x / len, y / len, z / len, w);
            return Vec4(0, 0, 0, w);
        }

        // Cross product — w ignored
        fgt_device Vec4 cross(const Vec4& o) const {
            return Vec4(
                y * o.z - z * o.y,
                z * o.x - x * o.z,
                x * o.y - y * o.x,
                0.0f
            );
        }

        // Index access
        fgt_device float  operator[](int i) const { if (i == 0) return x; if (i == 1) return y; if (i == 2) return z; return w; }
        fgt_device float& operator[](int i) { if (i == 0) return x; if (i == 1) return y; if (i == 2) return z; return w; }
    };
    fgt_device inline fungt::Vec3 sub(const Vec4& a, const Vec4& b)  {
        return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
    }
    //Mutiply a Vec4 times a scalar, returning a Vec3 (w ignored)
    fgt_device inline fungt::Vec3 multiply(const Vec4& v, float s) {
        return Vec3(v.x * s, v.y * s, v.z * s);
    }
}


#endif // _VEC4_H_
