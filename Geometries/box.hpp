#if !defined(_BOX_HPP_)
#define _BOX_HPP_

#include "primitives.hpp"

namespace geometry {

class Box : public Primitive {
private:
    float m_width;
    float m_height;
    float m_depth;

public:
    Box(float width = 1.0f, float height = 1.0f, float depth = 1.0f);
    ~Box();

    void draw() override;
    void setData() override;
    void IntancedDraw(Shader &shader, int instanceCount) override;
};

}

#endif // _BOX_HPP_
