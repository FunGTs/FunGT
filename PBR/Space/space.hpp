#if !defined(_SPACE_H_)
#define _SPACE_H_
#include<vector>
#include "../PBRCamera/pbr_camera.hpp"
#include "../../Triangle/triangle.hpp"
#include "../Light/light.hpp"
class Space {

    PBRCamera camera;
    std::vector<Triangle> m_triangles;
    std::vector<Light> m_lights; 



    public:
        Space();
        ~Space();




};




#endif // _SPACE_H_
