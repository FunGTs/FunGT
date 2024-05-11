#if !defined(_HELPERS_H_)
#define _HELPERS_H_
#include<assimp/quaternion.h>
#include<assimp/vector3.h>
#include<assimp/matrix4x4.h>
#include<glm/gtc/quaternion.hpp>
#include "../include/glmath.hpp"
#include "../include/prequisites.hpp"
namespace funGL
{
    class Helpers
    {
        public:
            static glm::mat4 convertMatToGlm(const aiMatrix4x4 &from);
            static  glm::vec3 gtGLMVec(const aiVector3D& vec);
            static  glm::quat getGLMQuat(const aiQuaternion& pOrientation); 

    };
        
    
} // namespace funGL


#endif // _HELPERS_H_
