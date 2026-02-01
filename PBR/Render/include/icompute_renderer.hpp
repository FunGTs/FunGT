#if !defined(_I_COMPUTE_RENDERER_H_)
#define _I_COMPUTE_RENDERER_H_
#include <vector>
#include "Triangle/triangle.hpp"
#include "Vector/vector3.hpp"
#include "PBR/BVH/bvh_node.hpp"
#include "PBR/Light/light.hpp"
// Forward declarations 
class PBRCamera;


class IComputeRenderer{

    public: 
        virtual ~IComputeRenderer() = default;
        virtual std::vector<fungt::Vec3> RenderScene(
            int width, int height,
            const std::vector<Triangle> &triangleList,
            const std::vector<BVHNode> &nodes,
            const std::vector<Light> &lightsList,
            const std::vector<int>& emissiveTriIndices,
            const PBRCamera& camera, 
            int samplesPerPixel 
        ) = 0;


};




#endif // _I_COMPUTE_RENDERER_H_
