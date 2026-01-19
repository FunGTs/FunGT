#if !defined(_SPHERE_HPP_)
#define _SPHERE_HPP_

#include "primitives.hpp"

namespace geometry{


    class Sphere : public Primitive {
    private:
        int m_sectorCount;  // Longitudinal divisions
        int m_stackCount;   // Latitudinal divisions
        float m_radius;

    public:
        Sphere(float radius = 1.0f, int sectorCount = 36, int stackCount = 18);
        ~Sphere();

        void draw() override;
        void setData() override;
        void IntancedDraw(Shader &shader, int instanceCount) override;
    };


}

#endif // _SPHERE_HPP_