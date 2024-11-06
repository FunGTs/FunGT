#include "matrix4x4f.h"

fungl::Matrix4f::Matrix4f()
{
    for (unsigned int i = 0 ; i < 4 ; i++) {
        for (unsigned int j = 0 ; j < 4 ; j++) {
            m[i][j] = 0.0f;
        }
    }
}
fungl::Matrix4f::Matrix4f(float a00, float a01, float a02, float a03,
                   float a10, float a11, float a12, float a13,
                   float a20, float a21, float a22, float a23,
                   float a30, float a31, float a32, float a33)
{
    m[0][0] = a00; m[0][1] = a01; m[0][2] = a02; m[0][3] = a03;
    m[1][0] = a10; m[1][1] = a11; m[1][2] = a12; m[1][3] = a13;
    m[2][0] = a20; m[2][1] = a21; m[2][2] = a22; m[2][3] = a23;
    m[3][0] = a30; m[3][1] = a31; m[3][2] = a32; m[3][3] = a33;
}
fungl::Matrix4f::Matrix4f(const Matrix4f &input)
{
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            m[i][j] = input[i][j];
        }
    }
}
fungl::Matrix4f::Matrix4f(const aiMatrix4x4 &AssimpMatrix)
{
    m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = AssimpMatrix.a4;
    m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = AssimpMatrix.b4;
    m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = AssimpMatrix.c4;
    m[3][0] = AssimpMatrix.d1; m[3][1] = AssimpMatrix.d2; m[3][2] = AssimpMatrix.d3; m[3][3] = AssimpMatrix.d4;
}
fungl::Matrix4f::Matrix4f(const aiMatrix3x3 &AssimpMatrix)
{
        m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = 0.0f;
        m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = 0.0f;
        m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = 0.0f;
        m[3][0] = 0.0f           ; m[3][1] = 0.0f           ; m[3][2] = 0.0f           ; m[3][3] = 1.0f;

}

void fungl::Matrix4f::identity()
{
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}
void fungl::Matrix4f::transformTranslation(float x, float y, float z)
{
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;

}

void fungl::Matrix4f::scaleTransform(float scaleX, float scaleY, float scaleZ)
{
    m[0][0] = scaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
    m[1][0] = 0.0f;   m[1][1] = scaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
    m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = scaleZ; m[2][3] = 0.0f;
    m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;

}

fungl::Matrix4f fungl::Matrix4f::operator*(const Matrix4f& Right) const
{
    Matrix4f Result;
    for (unsigned int i = 0 ; i < 4 ; i++) {
        for (unsigned int j = 0 ; j < 4 ; j++) {
            Result.m[i][j] = m[i][0] * Right.m[0][j] +
                             m[i][1] * Right.m[1][j] +
                             m[i][2] * Right.m[2][j] +
                             m[i][3] * Right.m[3][j];
        }
    }
    return Result;
}   
fungl::Matrix4f fungl::Matrix4f::Transpose() const
{
    Matrix4f Result;
    for (unsigned int i = 0 ; i < 4 ; i++) {
        for (unsigned int j = 0 ; j < 4 ; j++) {
            Result.m[i][j] = m[j][i];
        }
    }
    return Result;
}
void fungl::Matrix4f::print() const
{
     for (int i = 0 ; i < 4 ; i++) {
            printf("%f %f %f %f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
        }
}
glm::mat4 fungl::Matrix4fToGlmMat4(const Matrix4f &matrix)
{
    glm::mat4 result;
    for (unsigned int i = 0; i < 4; i++) {
        for (unsigned int j = 0; j < 4; j++) {
            result[j][i] = matrix.m[i][j];
        }
    }
    return result;
}
