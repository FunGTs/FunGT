#include "space.hpp"

Space::Space(){
    

    m_lights.push_back(Light(
        fungt::Vec3(2.0f, 2.0f, 2.0f),    // position
        fungt::Vec3(10.0f, 10.0f, 10.0f)  // strong white intensity
    ));

}

Space::~Space()
{
}
