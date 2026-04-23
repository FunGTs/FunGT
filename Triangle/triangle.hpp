#if !defined(_TRIANGLE_H_)
#define _TRIANGLE_H_
#include "Vector/vector4.hpp"
#include "gpu/data/device_pod.hpp"

namespace gpu{

    struct TriangleGeometry {
        fungt::Vec4 v0, v1, v2;  // 48 bytes
    };

    struct TriangleShadingData {
        fungt::Vec4  n0, n1, n2;  // 48 bytes
        MaterialData material;    // 32 bytes
        float        uvs[3][2];   // 24 bytes
        float        _pad[2];     // 8 bytes
    };                            // total 112 bytes

}



struct Triangle {
    fungt::Vec3 v0, v1, v2;
    fungt::Vec3  n0, n1,n2;
    float       uvs[3][2];
    MaterialData material;
};






#endif // _TRIANGLE_H_
