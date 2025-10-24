#include "../Space/space.hpp"


int main(){
    Space space;
    auto framebuffer = space.Render(400, 225);
    Space::SaveFrameBufferAsPNG(framebuffer, 400, 225);

    return 0;
}