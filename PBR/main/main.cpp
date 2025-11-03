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
    //ComputeRender::Init();
    ComputeRender::SetBackend(Compute::Backend::CUDA);
    std::cout << "Backend in use: " << ComputeRender::GetBackendName() << std::endl;

    std::shared_ptr<SimpleModel> monkey_model = SimpleModel::create();
    monkey_model->LoadModelData(monkey);
    
    std::vector<Triangle> triangleList = monkey_model->getTriangleList();

    std::cout<<"total triangles : "<<triangleList.size()<<std::endl;
    

    Space space(triangleList);
    space.setSamples(64);
    // //Space space;
    auto framebuffer = space.Render(IMAGE_WIDTH, IMAGE_HEIGHT);
    Space::SaveFrameBufferAsPNG(framebuffer, IMAGE_WIDTH, IMAGE_HEIGHT);

    std::cout << "total triangles after : " << triangleList.size() << std::endl;

    return 0;
}