#include <iostream>

template<class T>
class Box
{
private:
    T content;

public:
    void set(const T &newContent)
    {
        content = newContent;
    }

    T get() const
    {
        return content;
    }
};

int main()
{
    Box<int> intBox;
    intBox.set(123);
    std::cout << "Box content: " << intBox.get() << std::endl;

    Box<std::string> strBox;
    std::string msg = "hola";
    strBox.set(msg);
    std::cout << "Box content: " << strBox.get() << std::endl;


    Box<float> fBox;
    fBox.set(100.4);
    std::cout << "Box content: " << fBox.get() << std::endl;
    return 0;
}