#if !defined(_RAY_H_)
#define _RAY_H_
#include "../../Vector/vector3.hpp"
#include "../../gpu/include/fgt_cpu_device.hpp"
namespace fungt{

    class Ray{
        public:
            fungt::Vec3      m_origin;
            fungt::Vec3      m_dir;


        public:
            fgt_device Ray(){
            }
            fgt_device Ray(const fungt::Vec3& origin, const fungt::Vec3& dir)
            :m_origin{origin}, m_dir{dir}{
            }
            fgt_device ~Ray() {

            }
            fgt_device fungt::Vec3 at(float t) const {
                fungt::Vec3 res;
                return m_origin + m_dir*t;
                return res;
            }

    };






}




#endif // _RAY_H_
