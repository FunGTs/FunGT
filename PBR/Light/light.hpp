#if !defined(_LIGHT_H_)
#define _LIGHT_H_
#include "../../Vector/vector3.hpp"
enum class LightType{

    Point,
    Directional,
    Area
};


class Light{

        fungt::Vec3 m_pos;
        fungt::Vec3 m_intensity; 
        LightType m_type;
    public:
        Light(){
        
            m_pos       = fungt::Vec3{0.0f, 0.0f, 0.0f};
            m_intensity = fungt::Vec3{1.0f, 1.0f, 1.0f};
            m_type      = LightType::Point;

        }
        Light(const fungt::Vec3& pos, const fungt::Vec3& inten, LightType t = LightType::Point)
            : m_pos{pos}, m_intensity{inten}, m_type{t} {
        }


};



#endif // _LIGHT_H_

