#if !defined(_I_COMPUTE_RENDERER_H_)
#define _I_COMPUTE_RENDERER_H_
#include <vector>
#include "../../Triangle/triangle.hpp"
#include "../../Vector/vector3.hpp"
#include "../PBRCamera/pbr_camera.hpp"
class IComputeRenderer{

    public: 
        virtual ~IComputeRenderer() = default;
        virtual std::vector<fungt::Vec3> RenderScene(
            int width, int height,
            const std::vector<Triangle> &triangleList,
            const PBRCamera& camera, 
            int samplesPerPixel
        ) = 0;


};




#endif // _I_COMPUTE_RENDERER_H_
