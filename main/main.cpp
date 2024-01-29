#include "../App/fungl.hpp"



int main(){

    
    FunGL myWindow{1600,1000,"FunGL App"};
    myWindow.setBackground(0.0f, 0.f, 0.f, 1.0f);
    myWindow.set();

    return 0;
}