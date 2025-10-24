#include "space.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../vendor/stb_image/stb_image_write.h"
Space::Space(){
    
    Material gray(glm::vec3(0.2f), glm::vec3(0.8f), glm::vec3(0.8f), 32.0f, "DefaultGray");
    m_triangles = create_unit_cube(gray);

    m_lights.push_back(Light(
        fungt::Vec3(2.0f, 2.0f, 2.0f),    // position
        fungt::Vec3(10.0f, 10.0f, 10.0f)  // strong white intensity
    ));

}

Space::~Space(){
}

std::vector<fungt::Vec3> Space::Render(const int width, const int height) {
    std::vector<fungt::Vec3> framebuffer;
    framebuffer.resize(width*height);
    float aspectRatio = float(width) / float(height);

    PBRCamera cam(aspectRatio);


    for(int i = 0; i<height; i++){
        for(int j = 0; j<width; j++){

            float u = float(j)/(width-1);
            float v = float(i)/(height-1);
            fungt::Ray ray = cam.getRay(u, v);

            HitData hit_data;
            bool isHit = false;
            float closest = FLT_MAX;

            for (const auto& tri : m_triangles) {
                HitData tempData;
                if (Intersection::MollerTrumbore(ray, tri, 0.001f, closest, tempData)) {
                    isHit = true;
                    closest = tempData.dis;
                    hit_data = tempData;
                }
            }
            fungt::Vec3 color;
            if (isHit)
                color = shadeNormal(hit_data.normal);
            else
                color = fungt::Vec3(0.5f, 0.5f, 0.5f); // background

            framebuffer[i* width + j] = color;

        }

    }
    return framebuffer;

}

void Space::SaveFrameBufferAsPNG(const std::vector<fungt::Vec3>& framebuffer, int width, int height)
{
    std::vector<unsigned char> pixels(width * height * 3);

    for (int i = 0; i < width * height; i++) {
        fungt::Vec3 color = framebuffer[i];
        // Clamp and gamma correct
        color.x = std::pow(std::clamp(color.x, 0.0f, 1.0f), 1 / 2.2f);
        color.y = std::pow(std::clamp(color.y, 0.0f, 1.0f), 1 / 2.2f);
        color.z = std::pow(std::clamp(color.z, 0.0f, 1.0f), 1 / 2.2f);

        pixels[i * 3 + 0] = static_cast<unsigned char>(255.99f * color.x);
        pixels[i * 3 + 1] = static_cast<unsigned char>(255.99f * color.y);
        pixels[i * 3 + 2] = static_cast<unsigned char>(255.99f * color.z);
    }

    stbi_write_png("output.png", width, height, 3, pixels.data(), width * 3);
}



