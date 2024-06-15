#if !defined(_VERTEX_FUNGL_H_)
#define _VERTEX_FUNGL_H_

#include "../include/prequisites.hpp"
#include "../include/glmath.hpp"

const int maxBoneInfluencePerVertex = 4; 
struct Vertex
{
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
        //bone indexes which will influence this vertex
	int m_BoneIDs[maxBoneInfluencePerVertex];
	//weights from each bone
	float m_Weights[maxBoneInfluencePerVertex];
};
typedef struct Vertex funGTVERTEX;
#endif // _VERTEX_FUNGL_H_
