#include "funGT/fungt.hpp"

const unsigned int SCREEN_WIDTH = 1600;
const unsigned int SCREEN_HEIGHT = 1000;

int main() {

    DisplayGraphics::SetBackend(Backend::OpenGL);

    FunGTScene myGame = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    myGame->setBackgroundColor();
    myGame->initGL();

    myGame->set([&]() {
        });

    myGame->render([&]() {
        });

    return 0;
}
