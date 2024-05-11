#include "../funGT/fungt.hpp"

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1000;
int main(){

    
    FunGT myWindow{SCR_WIDTH,SCR_HEIGHT};
    myWindow.setBackground(255.f);
    myWindow.run();

    return 0;
}