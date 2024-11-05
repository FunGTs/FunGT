#include "helpers.hpp"

glm::mat4 funGL::Helpers::convertMatToGlm(const aiMatrix4x4 &from){
    glm::mat4 to;
    // //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    // to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    // to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    // to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    // to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

    to[0][0] = from.a1; to[0][1] = from.a2; to[0][2] = from.a3; to[0][3] = from.a4;
    to[1][0] = from.b1; to[1][1] = from.b2; to[1][2] = from.b3; to[1][3] = from.b4;
    to[2][0] = from.c1; to[2][1] = from.c2; to[2][2] = from.c3; to[2][3] = from.c4;
    to[3][0] = from.d1; to[3][1] = from.d2; to[3][2] = from.d3; to[3][3] = from.d4;
	return to;
}
glm::vec3 funGL::Helpers::gtGLMVec(const aiVector3D& vec){ 
	return glm::vec3(vec.x, vec.y, vec.z); 
}
glm::quat funGL::Helpers::getGLMQuat(const aiQuaternion& pOrientation){
	return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}

void funGL::Helpers::printAiMatrix4x4( const aiMatrix4x4 &mat)
{
    printf("%f %f %f %f\n", mat.a1, mat.a2, mat.a3, mat.a4);
    printf("%f %f %f %f\n", mat.b1, mat.b2, mat.b3, mat.b4);
    printf("%f %f %f %f\n", mat.c1, mat.c2, mat.c3, mat.c4);
    printf("%f %f %f %f\n", mat.d1, mat.d2, mat.d3, mat.d4);
}

void funGL::Helpers::printGlmMat4(glm::mat4 &mat)
{
    for (int i = 0 ; i < 4 ; i++) {
        printf("%f %f %f %f\n", mat[i][0], mat[i][1], mat[i][2], mat[i][3]);
    }
    
}