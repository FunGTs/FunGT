#if !defined(_DEVIVE_POD_H_)
#define _DEVIVE_POD_H_
#include "../include/fgt_cpu_device.hpp"
struct MaterialData {
    float   baseColor[3];   // Albedo in linear space (e.g. {0.8, 0.8, 0.8})
    float   metallic;    // 0 = dielectric, 1 = fully metallic

    float   roughness;   // 0 = mirror-smooth, 1 = rough
    float   reflectance; // F0 for dielectrics (typ. 0.04)
    float   emission;    // Intensity if the material emits light
    int   baseColorTexIdx;
    fgt_device MaterialData(){
        baseColor[0] = 0.8;
        baseColor[1] = 0.8;
        baseColor[2] = 0.8;
        metallic = 0.0f;
        roughness = 1.0f;
        reflectance = 0.04f;
        emission = 0.0f;
        baseColorTexIdx = -1;
    }
};
#endif // _DEVIVE_POD_H_
