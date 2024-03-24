#if !defined(_VERTEX_FUNGL_H_)
#define _VERTEX_FUNGL_H_

#include "../include/prequisites.hpp"
#include "../include/glmath.hpp"
#include<string>

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;  
    };
    typedef struct Vertex funGTVERTEX;
#endif // _VERTEX_FUNGL_H_
