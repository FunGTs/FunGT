#if !defined(_RAY_H_)
#define _RAY_H_
#include "../../Vector/vector3.hpp"
namespace fungt{

    class Ray{
        public:
            fungt::Vec3      m_origin;
            fungt::Vec3      m_dir;


        public:
            Ray(){
            }
            Ray(const fungt::Vec3& origin, const fungt::Vec3& dir)
            :m_origin{origin}, m_dir{dir}{
            }
            ~Ray(){

            }

            fungt::Vec3 at(float t) const {
                fungt::Vec3 res;
                return m_origin + m_dir*t;
                return res;
            }



    };






}




#endif // _RAY_H_
