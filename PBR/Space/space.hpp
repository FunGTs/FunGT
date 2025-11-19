#if !defined(_SPACE_H_)
#define _SPACE_H_
#include<vector>

#include "../../Triangle/triangle.hpp"
#include "../../SimpleModel/simple_model.hpp"
#include "../Intersection/intersection.hpp"
#include "../../SimpleModel/simple_model.hpp"
#include "../../Path_Manager/path_manager.hpp"
#include "../PBRCamera/pbr_camera.hpp"
#include "../../Random/random.hpp"
#include "defualt_geometries.hpp"
#include "../Light/light.hpp"
#include "../Render/include/compute_backends.hpp"
#include "../Render/include/icompute_renderer.hpp"
#include "../Render/include/cpu_renderer.hpp"
#include "../Render/include/cuda_renderer.hpp"
#include "../TextureManager/texture_manager.hpp"
#include "../TextureManager/cuda_texture.hpp"
#include "../TextureManager/cpu_texture.hpp"
#include <algorithm>
class Space {

    PBRCamera m_camera;
    std::vector<Triangle> m_triangles;
    std::unique_ptr<IComputeRenderer> m_computeRenderer;
    std::vector<Light> m_lights; 
    int m_samplesPerPixel = 16;
    std::shared_ptr<IDeviceTexture> m_textureManager;


    
    public:
        Space();
        Space(std::vector<Triangle>& triangleList);
        ~Space();

        std::vector<fungt::Vec3> Render(const int width, const int height);
        void LoadModelToRender(const SimpleModel& model);
        void static SaveFrameBufferAsPNG(const std::vector<fungt::Vec3>& framebuffer, int width, int height);
        void setSamples(int numOfSamples);


};




#endif // _SPACE_H_
