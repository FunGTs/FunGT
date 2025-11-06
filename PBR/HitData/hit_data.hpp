#if !defined(_HIT_DATA_H_)
#define _HIT_DATA_H_
#include "../../Vector/vector3.hpp"
#include "../../gpu/data/device_pod.hpp"
struct HitData {
    float dis;
    fungt::Vec3 point;
    fungt::Vec3 normal;
    MaterialData material;

};

#endif // _HIT_DATA_H_
