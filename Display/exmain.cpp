#include "display.hpp"



int main(){

    
    Display myWindow{800,600,"FunGL"};
    myWindow.setBackground(0.07f, 0.13f, 0.17f, 1.0f);
    myWindow.set();

    return 0;
}