#if !defined(_VERTEX_FUNGL_H_)
#define _VERTEX_FUNGL_H_

#include "../include/prerequisites.hpp"
#include "../include/glmath.hpp"
#include "../GraphicsRenderBackend/vertex_format.hpp"

const int maxBoneInfluencePerVertex = 4;
struct Vertex
{
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
        //bone indexes which will influence this vertex
	int m_BoneIDs[maxBoneInfluencePerVertex] = {-1,-1,-1,-1};
	//weights from each bone
	float m_Weights[maxBoneInfluencePerVertex] = {0.0f, 0.0f, 0.0f, 0.0f};

    FUNGT_VERTEX_FORMAT(Vertex, position, normal, texcoord, m_BoneIDs, m_Weights)
};
typedef struct Vertex funGTVERTEX;
#endif // _VERTEX_FUNGL_H_
