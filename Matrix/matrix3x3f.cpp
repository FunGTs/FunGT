#include "matrix3x3f.hpp"


fungt::Matrix3f::Matrix3f()
{
    for (unsigned int i = 0 ; i < 3 ; i++) {
        for (unsigned int j = 0 ; j < 3 ; j++) {
            m[i][j] = 0.0f;
        }
    }
}
fungt::Matrix3f::Matrix3f(float a00, float a01, float a02,
                         float a10, float a11, float a12,
                         float a20, float a21, float a22)
{
    m[0][0] = a00; m[0][1] = a01; m[0][2] = a02;
    m[1][0] = a10; m[1][1] = a11; m[1][2] = a12;
    m[2][0] = a20; m[2][1] = a21; m[2][2] = a22;
}
fungt::Matrix3f::Matrix3f(const Matrix3f &input)
{
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            m[i][j] = input.m[i][j];
        }
    }
}

fungt::Vec3 fungt::Matrix3f::operator*(const Vec3 &v) const
{
    return Vec3(
            m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z,
            m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z,
            m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z
        );
}

fungt::Matrix3f fungt::Matrix3f::operator*(const Matrix3f &other) const
{
     fungt::Matrix3f result;

    // Row 0
    result.m[0][0] = m[0][0] * other.m[0][0] + m[0][1] * other.m[1][0] + m[0][2] * other.m[2][0];
    result.m[0][1] = m[0][0] * other.m[0][1] + m[0][1] * other.m[1][1] + m[0][2] * other.m[2][1];
    result.m[0][2] = m[0][0] * other.m[0][2] + m[0][1] * other.m[1][2] + m[0][2] * other.m[2][2];

    // Row 1
    result.m[1][0] = m[1][0] * other.m[0][0] + m[1][1] * other.m[1][0] + m[1][2] * other.m[2][0];
    result.m[1][1] = m[1][0] * other.m[0][1] + m[1][1] * other.m[1][1] + m[1][2] * other.m[2][1];
    result.m[1][2] = m[1][0] * other.m[0][2] + m[1][1] * other.m[1][2] + m[1][2] * other.m[2][2];

    // Row 2
    result.m[2][0] = m[2][0] * other.m[0][0] + m[2][1] * other.m[1][0] + m[2][2] * other.m[2][0];
    result.m[2][1] = m[2][0] * other.m[0][1] + m[2][1] * other.m[1][1] + m[2][2] * other.m[2][1];
    result.m[2][2] = m[2][0] * other.m[0][2] + m[2][1] * other.m[1][2] + m[2][2] * other.m[2][2];

    return result;
}

fungt::Matrix3f fungt::Matrix3f::transpose() const
{
    fungt::Matrix3f result;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            result.m[i][j] = m[j][i];
        }
    }
    return result;
}
