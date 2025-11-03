#if !defined(_PBR_CAMERA_H_)
#define _PBR_CAMERA_H_
#include "../../Vector/vector3.hpp"
#include "../Ray/ray.hpp"
//This camera is different from the main camera

class PBRCamera{

        fungt::Vec3 origin;
        fungt::Vec3 lowerLeftCorner;
        fungt::Vec3 horizontal;
        fungt::Vec3 vertical;

    public:
        fgt_device PBRCamera(float aspectRatio = 16.0 / 9.0, float viewportHeight = 2.0f, float focalLength = 1.0f) {
            float viewportWidth = aspectRatio * viewportHeight;
            origin = fungt::Vec3(-5, 5, 12);
            horizontal = fungt::Vec3(viewportWidth, 0, 0);
            vertical = fungt::Vec3(0, viewportHeight, 0);
            lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - fungt::Vec3(0, 0, focalLength);
        }
        fgt_device fungt::Ray getRay(float u, float v) const {
            
            fungt::Vec3 dir = lowerLeftCorner + u * horizontal + v * vertical - origin;
            return fungt::Ray(origin, dir.normalize());
        }

};




#endif // _PBR_CAMERA_H_
