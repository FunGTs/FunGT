#ifndef MATRIX4X4F_H
#define MATRIX4X4F_H
#include <assimp/matrix4x4.h>
#include <assimp/matrix3x3.h>
#include "../include/glmath.hpp"
#include <iostream>
namespace fungl{


    class Matrix4f
    {
    public:
        float m[4][4];

        Matrix4f();

        Matrix4f(float a00, float a01, float a02, float a03,
                float a10, float a11, float a12, float a13,
                float a20, float a21, float a22, float a23,
                float a30, float a31, float a32, float a33);
        Matrix4f(const Matrix4f& input);

        // constructor from Assimp matrix
        Matrix4f(const aiMatrix4x4& AssimpMatrix);
        Matrix4f(const aiMatrix3x3& AssimpMatrix);
        
        Matrix4f Transpose() const;
        void print()const;
        void identity();
        void transformTranslation(float x, float y, float z);
        void scaleTransform(float scaleX,float scaleY, float scaleZ);

        Matrix4f operator*(const Matrix4f& Right) const;
         // Overload the [] operator to return a row
        float* operator[](int index) {
            return m[index];
        }

        // Overload the [] operator to return a const row for const objects
        const float* operator[](int index) const {
            return m[index];
        }
    

    };
    glm::mat4 Matrix4fToGlmMat4(const Matrix4f& matrix);

}

#endif // MATRIX4X4F_H