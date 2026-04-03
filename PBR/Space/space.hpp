#if !defined(_SPACE_H_)
#define _SPACE_H_
#include<vector>

#include "Triangle/triangle.hpp"
#include "SimpleModel/simple_model.hpp"
#include "PBR/Intersection/intersection.hpp"
#include "SimpleModel/simple_model.hpp"
#include "Path_Manager/path_manager.hpp"
#include "Random/random.hpp"
#include "defualt_geometries.hpp"
#include "Lights/scene_light.hpp"
#include "PBR/Light/light.hpp"
#include "PBR/Render/include/compute_backends.hpp"
#include "PBR/Render/include/icompute_renderer.hpp"
#include "PBR/Render/include/cpu_renderer.hpp"

#include "PBR/TextureManager/idevice_texture.hpp"

#include "PBR/TextureManager/cpu_texture.hpp"
#include "PBR/BVH/bvh_builder.hpp"
#include "SimpleGeometry/simple_geometry.hpp"
#include <algorithm>

// Conditional includes - only include backend headers when enabled
#ifdef FUNGT_USE_CUDA
#include "PBR/Render/include/cuda_renderer.hpp"
#include "PBR/TextureManager/cuda_texture.hpp"
#endif
#ifdef FUNGT_USE_SYCL
#include "PBR/Render/include/sycl_renderer.hpp"
#include "PBR/TextureManager/sycl_texture.hpp"
#endif
#include "PBR/PBRCamera/pbr_camera.hpp"
class Space {

    PBRCamera m_camera;
    std::vector<Triangle> m_triangles;
    std::unique_ptr<IComputeRenderer> m_computeRenderer;
    std::vector<Light> m_lights; 
    int m_samplesPerPixel = 16;
    std::shared_ptr<IDeviceTexture> m_textureManager;
    std::vector<BVHNode> m_bvh_nodes;
    std::vector<int>     m_bvh_indices;
    std::vector<int> m_emissiveTriIndices;
    
    void sendTexturesToRender();

    public:
        Space();
        Space(std::vector<Triangle>& triangleList);
        Space(const PBRCamera &camera);
        ~Space();

        std::vector<fungt::Vec3> Render(const int width, const int height,int sampleOffset = 0);
       
        void InitComputeRenderBackend();
        void LoadModelToRender(const SimpleModel& model);
        void LoadGeometryToRender(const SimpleGeometry& geometry);
        void loadLightsFromScene(const std::vector<SceneLight>& sceneLights);
        void static SaveFrameBufferAsPNG(const std::vector<fungt::Vec3>& framebuffer, int width, int height);
        static void SaveFrameBufferAsPNG(const std::vector<fungt::Vec3>& framebuffer,
            int width, int height,
            const std::string& filename);
        void BuildBVH();
        void setSamples(int numOfSamples);
        void ClearSpace() {
            m_triangles.clear();
            m_bvh_nodes.clear();
            m_bvh_indices.clear();
            m_emissiveTriIndices.clear();
            m_lights.clear();
        }
        void ClearGeometry() {
            m_triangles.clear();
            m_bvh_nodes.clear();
            m_bvh_indices.clear();
            m_emissiveTriIndices.clear();
        }
        void LoadTrianglesToRender(const std::vector<Triangle>& triangles) {
            m_triangles = triangles;
        }
        std::shared_ptr<IDeviceTexture> getTextureManager() const {
            return m_textureManager;
        }


};




#endif // _SPACE_H_
