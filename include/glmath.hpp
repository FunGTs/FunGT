#if !defined(_GLMMATH_H_)
#define _GLMMATH_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 

#ifdef _WIN32
    // Windows uses vcpkg-installed GLM (new version)
    #define NOMINMAX  // Prevents Windows headers from defining min/max
    #include <glm/ext/matrix_transform.hpp>
    #include <glm/ext/matrix_clip_space.hpp>
    #include <glm/ext/scalar_constants.hpp>
    #include <glm/ext/vector_float2.hpp>
    #include <glm/ext/vector_float3.hpp>
    #include <glm/ext/vector_float4.hpp>
    #include <glm/ext/matrix_float4x4.hpp>
    #include <glm/ext/quaternion_float.hpp>
#else
    // Linux uses system-installed GLM (older version)
    #define GLM_ENABLE_EXPERIMENTAL
    #include <glm/ext/matrix_transform.hpp>
    #include <glm/ext/matrix_projection.hpp>
    #include <glm/ext/scalar_constants.hpp>
    #include <glm/vec2.hpp>
    #include <glm/vec3.hpp>
    #include <glm/vec4.hpp>
    #include <glm/mat4x4.hpp>
    #include <glm/ext/quaternion_float.hpp>
#endif

#endif // _GLMMATH_H_
