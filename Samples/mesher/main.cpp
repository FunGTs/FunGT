#include "funGT/fungt.hpp"

const unsigned int SCREEN_WIDTH = 2100;
const unsigned int SCREEN_HEIGHT = 1200;

int main()
{
    DisplayGraphics::SetBackend(Backend::OpenGL);

    FunGTScene myScene = FunGT::createScene(SCREEN_WIDTH, SCREEN_HEIGHT);
    myScene->setBackgroundColor();
    myScene->initGL();

    FunGTSceneManager scene_manager = myScene->getSceneManager();

    // load density grid
    std::vector<float> density(128 * 128 * 128);
    std::ifstream f("ref_triposr_density.bin", std::ios::binary);
    
    if (!f.is_open()) {
        std::cerr << "Failed to open final_dens.bin" << std::endl;
    }
    f.read((char*)density.data(), density.size() * sizeof(float));
    int res = 128;
    float radius = 0.87f;
    std::vector<float> sorted_density = density;
    std::sort(sorted_density.begin(), sorted_density.end());
    int n = sorted_density.size();
    std::cout << "density min: " << sorted_density[0] << std::endl;
    std::cout << "density max: " << sorted_density[n - 1] << std::endl;
    std::cout << "50%: " << sorted_density[n * 50 / 100] << std::endl;
    std::cout << "90%: " << sorted_density[n * 90 / 100] << std::endl;
    std::cout << "95%: " << sorted_density[n * 95 / 100] << std::endl;
    std::cout << "99%: " << sorted_density[n * 99 / 100] << std::endl;
    std::cout << "99.5%: " << sorted_density[n * 995 / 1000] << std::endl;
    std::cout << "99.9%: " << sorted_density[n * 999 / 1000] << std::endl;
    auto field = [&density, res, radius](fungt::Vec3 p) -> float {
        int ix = std::clamp((int)((p.x + radius) / (2.0f * radius) * (res - 1) + 0.5f), 0, res - 1);
        int iy = std::clamp((int)((p.y + radius) / (2.0f * radius) * (res - 1) + 0.5f), 0, res - 1);
        int iz = std::clamp((int)((p.z + radius) / (2.0f * radius) * (res - 1) + 0.5f), 0, res - 1);
        return density[iz * res * res + iy * res + ix];
    };


    auto surface = Surface::create();
    surface->setField(field);
    surface->setIsovalue(10.0f);
    surface->sampleScalarFieldInside(
        fungt::Vec3(-radius, -radius, -radius),
        fungt::Vec3(radius, radius, radius)
    );
    surface->setResolution(128, 128, 128);
    surface->load();
    myScene->set([&]() {
        scene_manager->addRenderableObj(surface);
    });

    myScene->render([&]() {
    });

    return 0;
}
