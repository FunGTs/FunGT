#if !defined(_TORUS_HPP_)
#define _TORUS_HPP_

#include "primitives.hpp"

class Torus : public Primitive {
private:
    float m_majorRadius;    // Distance from center to tube center
    float m_minorRadius;    // Tube thickness
    int m_majorSegments;    // Divisions around major circle
    int m_minorSegments;    // Divisions around tube

public:
    Torus(float majorRadius = 1.0f,
        float minorRadius = 0.3f,
        int majorSegments = 48,
        int minorSegments = 24);
    ~Torus();

    void draw() override;
    void setData() override;
    void InstancedDraw(Shader& shader, int instanceCount) override;

    // Parameter access (if you want to adjust after creation)
    void setRadii(float major, float minor);
    void setSegments(int major, int minor);
};

#endif // _TORUS_HPP_