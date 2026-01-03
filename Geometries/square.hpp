#if !defined(_Square_H_)
#define _Square_H_
#include "primitives.hpp"
class Square : public Primitive{

public:
    Square();
    Square(const std::string  &path);
    ~Square();

    void draw() override;
    void create(const std::string &pathToTexture) override;
    void setData() override;




};


#endif // _Square_H_
