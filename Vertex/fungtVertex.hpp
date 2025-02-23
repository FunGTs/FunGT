#if !defined(_VERTEX_FUNGL_H_)
#define _VERTEX_FUNGL_H_

#include "../include/prerequisites.hpp"
#include "../include/glmath.hpp"

const int maxBoneInfluencePerVertex = 4; 
struct Vertex
{
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
        //bone indexes which will influence this vertex
	int m_BoneIDs[maxBoneInfluencePerVertex] = {-1,-1,-1,-1};;
	//weights from each bone
	float m_Weights[maxBoneInfluencePerVertex] = {0.0f, 0.0f, 0.0f, 0.0f};
};
typedef struct Vertex funGTVERTEX;
#endif // _VERTEX_FUNGL_H_
