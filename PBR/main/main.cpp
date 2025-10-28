#include "../Space/space.hpp"
const int IMAGE_WIDTH = 800;
const int IMAGE_HEIGHT = 400; 

int main(){

    //Path to your shaders and models:
    ModelPaths monkey;
    DisplayGraphics::SetBackend(Backend::OpenGL);
    monkey.path    = getAssetPath("Obj/monkey/monkeyobj.obj"); 
    monkey.vs_path = getAssetPath("resources/luxolamp_vs.glsl");
    monkey.fs_path = getAssetPath("resources/luxolamp_fs.glsl");


    std::shared_ptr<SimpleModel> monkey_model = SimpleModel::create();
    monkey_model->LoadModel(monkey);
    std::vector<Triangle> triangleList = monkey_model->getTriangleList();

    std::cout<<"total triangles : "<<triangleList.size()<<std::endl;
    

    Space space;
    auto framebuffer = space.Render(IMAGE_WIDTH, IMAGE_HEIGHT);
    Space::SaveFrameBufferAsPNG(framebuffer, IMAGE_WIDTH, IMAGE_HEIGHT);

    return 0;
}