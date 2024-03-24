#include "../funGT/fungt.hpp"

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1000;
int main(){

    
    FunGT myWindow{SCR_WIDTH,SCR_HEIGHT};
    myWindow.setBackground(0.0f, 0.f, 0.f, 1.0f);
    myWindow.run();

    return 0;
}