#if !defined(_Square_H_)
#define _Square_H_
#include "primitives.hpp"
class Square : public Primitive{

public:
    Square();
    ~Square();
    void draw() override;
    void setData() override;
};


#endif // _Square_H_
