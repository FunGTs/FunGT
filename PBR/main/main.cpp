#include "../Space/space.hpp"
const int IMAGE_WIDTH = 800;
const int IMAGE_HEIGHT = 400; 

int main(){

    //Path to your shaders and models:
    ModelPaths monkey;
    DisplayGraphics::SetBackend(Backend::OpenGL);
    monkey.path    = getAssetPath("Obj/monkey/monkeyobj.obj"); 
    monkey.vs_path = getAssetPath("resources/monkey_vs.glsl.glsl");
    monkey.fs_path = getAssetPath("resources/monkey_fs.glsl.glsl");
    //ComputeRender::Init();
    
    //CUDA backend:
    ComputeRender::SetBackend(Compute::Backend::CUDA);
    std::cout << "Backend in use: " << ComputeRender::GetBackendName() << std::endl;
    //Model loading:
    std::shared_ptr<SimpleModel> monkey_model = SimpleModel::create();
    monkey_model->LoadModelData(monkey);
    
    std::vector<Triangle> triangleList = monkey_model->getTriangleList();

    std::cout<<"Total triangles : "<<triangleList.size()<<std::endl;
    

    Space space(triangleList);
    space.setSamples(64);
    auto framebuffer = space.Render(IMAGE_WIDTH, IMAGE_HEIGHT);
    Space::SaveFrameBufferAsPNG(framebuffer, IMAGE_WIDTH, IMAGE_HEIGHT);

    return 0;
}