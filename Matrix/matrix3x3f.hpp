#if !defined(_MATRIX3X3F_HPP_ )
#define _MATRIX3X3F_HPP_
#include "../Vector/vector3.hpp"

namespace fungl {

    class Matrix3f {
    public:
        float m[3][3];

        Matrix3f();

        Matrix3f(float a00, float a01, float a02,
                 float a10, float a11, float a12,
                 float a20, float a21, float a22);

        Matrix3f(const Matrix3f& input);
        Vec3 operator*(const Vec3& v) const;
        Matrix3f operator*(const Matrix3f& other) const;
        Matrix3f transpose() const;
    };
}

#endif // _MATRIX3X3F_HPP_
