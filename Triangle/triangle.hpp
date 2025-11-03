#if !defined(_TRIANGLE_H_)
#define _TRIANGLE_H_
#include "../../Vector/vector3.hpp"
#include "../gpu/data/device_pod.hpp"
struct Triangle {
    fungt::Vec3 v0, v1, v2;
    fungt::Vec3 normal;
    MaterialData material;
};






#endif // _TRIANGLE_H_
