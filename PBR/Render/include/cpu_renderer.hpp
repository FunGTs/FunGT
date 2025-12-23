#if !defined(_CPU_RENDERER)
#define _CPU_RENDERER
#include "icompute_renderer.hpp"
#include "../../Random/random.hpp"
#include "../Ray/ray.hpp"
#include "../HitData/hit_data.hpp"
#include "PBR/Intersection/intersection.hpp"


class CPU_Renderer : public IComputeRenderer{

    public:
        fungt::Vec3 shadeNormal(const fungt::Vec3& normal) {
            // Convert from [-1,1] to [0,1]
            //return 0.5f * (normal + fungt::Vec3(1.0f, 1.0f, 1.0f));
            float intensity = std::abs(normal.dot(fungt::Vec3(0, 0, 1))); // dot with light direction
            //return fungt::Vec3(0.2f, 0.2f, 0.2f) + 0.6f * intensity; // gray + simple diffuse
            return fungt::Vec3(0.2f + 0.6f * intensity,
                0.2f + 0.6f * intensity,
                0.2f + 0.6f * intensity);

        }
        std::vector<fungt::Vec3> RenderScene(
            int width, int height,
            const std::vector<Triangle>& triangleList,
            const std::vector<BVHNode> &nodes,
            const std::vector<Light> &lightsList,
            const PBRCamera& camera,
            int samplesPerPixel
        );


};



#endif // _CPU_RENDERER
