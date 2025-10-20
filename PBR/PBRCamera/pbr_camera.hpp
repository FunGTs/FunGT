#if !defined(_PBR_CAMERA_H_)
#define _PBR_CAMERA_H_
#include "../../Vector/vector3.hpp"
//This camera is different from the main camera

class PBRCamera{

        fungt::Vec3 origin;
        fungt::Vec3 pixel00_loc;
        fungt::Vec3 delta_u, delta_v;

    public:
        PBRCamera() {
            origin = fungt::Vec3(0, 0, 0);
            float aspect_ratio = 16.0 / 9.0;
            int image_width = 400;
            int image_height = int(image_width / aspect_ratio);
            float viewport_height = 2.0;
            float viewport_width = viewport_height * aspect_ratio;
            float focal_length = 1.0;

            fungt::Vec3 viewport_u(viewport_width, 0, 0);
            fungt::Vec3 viewport_v(0, -viewport_height, 0);

            delta_u = viewport_u / float(image_width);
            delta_v = viewport_v / float(image_height);

            pixel00_loc = origin - fungt::Vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2 + 0.5f * (delta_u + delta_v);
        }

        fungt::Vec3 getRayDir(int i, int j) const {

            float fi = static_cast<float>(i);
            float fj = static_cast<float>(j);
            fungt::Vec3 res;
            res = (pixel00_loc + float(i) * delta_u + float(j) * delta_v - origin).normalize();
            return res; 
        }



};




#endif // _PBR_CAMERA_H_
