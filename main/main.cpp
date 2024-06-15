#include "../funGT/fungt.hpp"

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1000;
int main(){

    
    FunGT myWindow{SCR_WIDTH,SCR_HEIGHT};
    //use 255.f fpr pure white, 
    myWindow.setBackground();
    myWindow.run();

    return 0;
}