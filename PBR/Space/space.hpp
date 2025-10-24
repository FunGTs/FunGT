#if !defined(_SPACE_H_)
#define _SPACE_H_
#include<vector>
#include "../../Triangle/triangle.hpp"
#include "../Intersection/intersection.hpp"
#include "../PBRCamera/pbr_camera.hpp"
#include "defualt_geometries.hpp"
#include "../Light/light.hpp"
#include <algorithm>
class Space {

    PBRCamera camera;
    std::vector<Triangle> m_triangles;
    std::vector<Light> m_lights; 



    public:
        Space();
        ~Space();

        std::vector<fungt::Vec3> Render(const int width, const int height);
        void static SaveFrameBufferAsPNG(const std::vector<fungt::Vec3>& framebuffer, int width, int height);

    private:
        fungt::Vec3 shadeNormal(const fungt::Vec3& normal) {
            // Convert from [-1,1] to [0,1]
            //return 0.5f * (normal + fungt::Vec3(1.0f, 1.0f, 1.0f));
            float intensity = std::abs(normal.dot(fungt::Vec3(0, 0, 1))); // dot with light direction
            //return fungt::Vec3(0.2f, 0.2f, 0.2f) + 0.6f * intensity; // gray + simple diffuse
            return fungt::Vec3(0.2f + 0.6f * intensity,
                0.2f + 0.6f * intensity,
                0.2f + 0.6f * intensity);

        }


};




#endif // _SPACE_H_
