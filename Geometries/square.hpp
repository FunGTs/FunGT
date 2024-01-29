#if !defined(_Square_H_)
#define _Square_H_
#include "primitives.hpp"
class Square : public Primitive{

public: 
    Square();
    Square(const std::string  &path);
    ~Square();

    void draw();




};


#endif // _Square_H_
