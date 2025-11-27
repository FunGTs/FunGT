#include "../Space/space.hpp"
const int IMAGE_WIDTH = 800;
const int IMAGE_HEIGHT = 400; 

int main(){

    //Path to your shaders and models:
    ModelPaths monkey;
    DisplayGraphics::SetBackend(Backend::OpenGL);
    monkey.path    = getAssetPath("Obj/Woody/woody-head.obj"); 
    monkey.vs_path = getAssetPath("resources/luxoball_vs.glsl");
    monkey.fs_path = getAssetPath("resources/luxoball_fs.glsl");
    //ComputeRender::Init();
    
    //CUDA backend:
    ComputeRender::SetBackend(Compute::Backend::CUDA);
    std::cout << "Backend in use: " << ComputeRender::GetBackendName() << std::endl;
    //Model loading:
    std::shared_ptr<SimpleModel> monkey_model = SimpleModel::create();
    monkey_model->LoadModelData(monkey);

    

    //std::shared_ptr<ITextureManager<cudaTextureObject_t>> txtMgr = std::make_shared<CUDATexture>();
    // std::vector<Triangle> triangleList = monkey_model->getTriangleList();
    // std::cout<<"Total triangles : "<<triangleList.size()<<std::endl;
    // std::cout << "HOST first tri diffuse: "
    //     << triangleList[0].material.baseColor[0] << ","
    //     << triangleList[0].material.baseColor[1] << ","
    //     << triangleList[0].material.baseColor[2] << std::endl;

    // Nice portrait angle for Woody
    PBRCamera camera(
        fungt::Vec3(0, 2.5, 30),     // Position (slightly above, in front)
        fungt::Vec3(0, 1.8, 0),     // Look at Woody's face
        fungt::Vec3(0, 1, 0),       // Up vector
        50.0f,                       // FOV (50° is good for portraits)
        float(IMAGE_WIDTH) / float(IMAGE_HEIGHT)
    );
    Space space(camera);
    space.LoadModelToRender(*monkey_model);
    space.setSamples(256);
    space.BuildBVH();
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