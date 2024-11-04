#include <iostream>

class Point
{
private:
    int x, y;

public:
    Point(int x, int y) : x(x), y(y) {}

    // Inserta aqui tu codigo

    void display() const
    {
        std::cout << "Point(" << x << ", " << y << ")" << std::endl;
    }
    Point operator+(const Point &p) const {

        
        return Point(x + p.x , y + p.y);
    }
};

int main()
{
    Point p1(1, 2), p2(3, 4);
    Point p3 = p1+p2; 
    
    p3.display();
    

    return 0;
}