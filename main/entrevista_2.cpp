#include <iostream>
#include <memory>
class Base
{
public:
    virtual void func() { std::cout << "Function in Base" << std::endl; }
};

class Derived : public Base
{
public:
    void func() override { std::cout << "Function in Derived" << std::endl; }
};

int main()
{


    //Inserta tu código aqui

    std::unique_ptr<Base> pointer; 

    pointer = std::make_unique<Derived>();

    pointer->func();

    return 0;
}