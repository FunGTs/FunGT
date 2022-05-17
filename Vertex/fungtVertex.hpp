#if !defined(_VERTEX_FUNGL_H_)
#define _VERTEX_FUNGL_H_
#include "../include/prequisites.hpp"
#include "../include/glmath.hpp"


    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec2 texcoord;
        glm::vec3 normal;     
    };
    typedef struct Vertex VERTEX;


#endif // _VERTEX_FUNGL_H_
