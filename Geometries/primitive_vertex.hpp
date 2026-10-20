#if !defined(_PRIMITIVE_VERTEX_DEF_HPP_)
#define _PRIMITIVE_VERTEX_DEF_HPP_

#include "GraphicsRenderBackend/vertex_format.hpp"
struct PrimitiveVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;

    FUNGT_VERTEX_FORMAT(PrimitiveVertex, position, normal, texcoord)
};

#endif // _PRIMITIVE_VERTEX_DEF_HPP_
