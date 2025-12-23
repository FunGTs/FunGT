#if !defined(_HELPERS_H_)
#define _HELPERS_H_
#include<assimp/quaternion.h>
#include<assimp/vector3.h>
#include<assimp/matrix4x4.h>
#include "include/glmath.hpp"
#include "include/prerequisites.hpp"
#include "Matrix/matrix4x4f.h"

const float EPSILON = 1e-6f; //For floating point comparison

namespace funGL
{
    class Helpers
    {
        public:
            static glm::mat4 convertMatToGlm(const aiMatrix4x4 &from);
            static glm::vec3 gtGLMVec(const aiVector3D& vec);
            static glm::quat getGLMQuat(const aiQuaternion& pOrientation);
            static void printAiMatrix4x4( const aiMatrix4x4 &mat);
            static void printGlmMat4(glm::mat4 &mat);

    };
        
    
} // namespace funGL


#endif // _HELPERS_H_
