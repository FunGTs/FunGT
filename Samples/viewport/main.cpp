#include "../../funGT/fungt.hpp"
const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;



int main (){
    
    //Creates a FunGT Scene to display 
    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    //Background color, use 255.f for pure white, 
    myGame->setBackgroundColor();
    //Initializes the Graphics Stuff
    myGame->initGL();
    myGame->set([&]() { // Sets up all the scenes in your game

    });
    myGame->render([&](){});

    return 0; 
    
}