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
    std::cout << "HOST first tri diffuse: "
        << triangleList[0].material.baseColor[0] << ","
        << triangleList[0].material.baseColor[1] << ","
        << triangleList[0].material.baseColor[2] << std::endl;


    Space space(triangleList);
    space.setSamples(64);
    auto totalStart = std::chrono::high_resolution_clock::now();
    auto framebuffer = space.Render(IMAGE_WIDTH, IMAGE_HEIGHT);
    auto totalEnd = std::chrono::high_resolution_clock::now();
   
    Space::SaveFrameBufferAsPNG(framebuffer, IMAGE_WIDTH, IMAGE_HEIGHT);

    auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(totalEnd - totalStart).count();
    // ============ PRINT RESULTS ============
    std::cout << "\n========== TIMING RESULTS ==========" << std::endl;
    std::cout << "Total time:       " << totalTime << " ms" << std::endl;
    std::cout << "====================================\n" << std::endl;

    return 0;
}