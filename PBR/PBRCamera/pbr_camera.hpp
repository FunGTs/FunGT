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
        fgt_device PBRCamera(const PBRCamera &_camera){
            this->horizontal = _camera.horizontal;
            this->lowerLeftCorner  = _camera.lowerLeftCorner;
            this->origin = _camera.origin;
            this->vertical = _camera.vertical;
        }
        fgt_device PBRCamera(float aspectRatio = 16.0 / 9.0, float viewportHeight = 2.0f, float focalLength = 1.0f) {
             float viewportWidth = aspectRatio * viewportHeight;
            origin = fungt::Vec3(0, 5,20);
            horizontal = fungt::Vec3(viewportWidth, 0, 0);
            vertical = fungt::Vec3(0, viewportHeight, 0);
            lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - fungt::Vec3(0, 0, focalLength);


            // // Diagonal view position (Luxo ball angle)
            // origin = fungt::Vec3(2.5f, 1.5f, 2.5f);

            // // Look-at calculation
            // fungt::Vec3 target(0.0f, 0.0f, 0.0f);  // Ball at origin
            // fungt::Vec3 worldUp(0.0f, 1.0f, 0.0f);

            // fungt::Vec3 forward = (target - origin).normalize();
            // fungt::Vec3 right = forward.cross(worldUp).normalize();
            // fungt::Vec3 up = right.cross(forward);

            // horizontal = right * viewportWidth;
            // vertical = up * viewportHeight;
            // lowerLeftCorner = origin + forward * focalLength - horizontal / 2 - vertical / 2;
        }
        fgt_device fungt::Ray getRay(float u, float v) const {
            
            fungt::Vec3 dir = lowerLeftCorner + u * horizontal + v * vertical - origin;
            return fungt::Ray(origin, dir.normalize());
        }

};




#endif // _PBR_CAMERA_H_
