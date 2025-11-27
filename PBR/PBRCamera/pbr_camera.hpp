#if !defined(_PBR_CAMERA_H_)
#define _PBR_CAMERA_H_
#include "../../Vector/vector3.hpp"
#include "../Ray/ray.hpp"
//This camera is different from the main camera
class PBRCamera {
private:
    fungt::Vec3 origin;
    fungt::Vec3 lowerLeftCorner;
    fungt::Vec3 horizontal;
    fungt::Vec3 vertical;
    fungt::Vec3 u, v, w;  // Camera basis vectors
    float lensRadius;

public:
    // Default constructor
    fgt_device PBRCamera() {
        // Simple default
        origin = fungt::Vec3(0, 0, 5);
        lowerLeftCorner = fungt::Vec3(-2, -1.5, -1);
        horizontal = fungt::Vec3(4, 0, 0);
        vertical = fungt::Vec3(0, 3, 0);
        lensRadius = 0;
    }

    // Copy constructor
    fgt_device PBRCamera(const PBRCamera& _camera) {
        this->origin = _camera.origin;
        this->lowerLeftCorner = _camera.lowerLeftCorner;
        this->horizontal = _camera.horizontal;
        this->vertical = _camera.vertical;
        this->u = _camera.u;
        this->v = _camera.v;
        this->w = _camera.w;
        this->lensRadius = _camera.lensRadius;
    }

    // PROPER CONSTRUCTOR
    fgt_device PBRCamera(
        fungt::Vec3 lookFrom,      // Camera position
        fungt::Vec3 lookAt,        // Point camera looks at
        fungt::Vec3 vup,           // Up vector (usually 0,1,0)
        float vfov,                // Vertical field of view in degrees
        float aspectRatio,         // Width / height
        float aperture = 0.0f,     // Aperture size (0 = pinhole, no DOF)
        float focusDist = 1.0f     // Focus distance
    ) {
        // Convert FOV to radians
        float theta = vfov * M_PI / 180.0f;
        float h = tanf(theta / 2.0f);
        float viewportHeight = 2.0f * h * focusDist;
        float viewportWidth = aspectRatio * viewportHeight;

        // Calculate camera basis vectors (right-handed coordinate system)
        w = (lookFrom - lookAt).normalize();  // Forward (away from lookAt)
        u = vup.cross(w).normalize();         // Right
        v = w.cross(u);                       // Up

        origin = lookFrom;
        horizontal = u * viewportWidth;
        vertical = v * viewportHeight;
        lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - w * focusDist;

        lensRadius = aperture / 2.0f;
    }
    // Assignment operator 
    fgt_device PBRCamera& operator=(const PBRCamera& other) {
        if (this != &other) {  // Self-assignment check
            origin = other.origin;
            lowerLeftCorner = other.lowerLeftCorner;
            horizontal = other.horizontal;
            vertical = other.vertical;
            u = other.u;
            v = other.v;
            w = other.w;
            lensRadius = other.lensRadius;
        }
        return *this;
    }
    // Simple constructor (backward compatible)
    fgt_device PBRCamera(float aspectRatio, float viewportHeight = 2.0f, float focalLength = 1.0f) {
        float viewportWidth = aspectRatio * viewportHeight;
        origin = fungt::Vec3(0, 5, 20);
        horizontal = fungt::Vec3(viewportWidth, 0, 0);
        vertical = fungt::Vec3(0, viewportHeight, 0);
        lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - fungt::Vec3(0, 0, focalLength);
    }

    // Get ray (with optional depth of field)
    fgt_device fungt::Ray getRay(float u, float v) const {
        fungt::Vec3 rd = randomInUnitDisk() * lensRadius;
        fungt::Vec3 offset = u * rd.x + v * rd.y;

        fungt::Vec3 direction = lowerLeftCorner + u * horizontal + v * vertical - origin - offset;
        return fungt::Ray(origin + offset, direction.normalize());
    }

private:
    // Random point in unit disk (for depth of field)
    fgt_device fungt::Vec3 randomInUnitDisk() const {
        // For now, return zero (no DOF)
        // You can add proper random sampling later with your RNG
        return fungt::Vec3(0, 0, 0);
    }
};
#endif // _PBR_CAMERA_H_
