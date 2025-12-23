#include "PBR/Render/include/cpu_renderer.hpp"
#include "PBR/PBRCamera/pbr_camera.hpp"
#include "cpu_renderer.hpp"



std::vector<fungt::Vec3> CPU_Renderer::RenderScene(int width, int height, const std::vector<Triangle>& triangleList, const std::vector<BVHNode>& nodes, const std::vector<Light>& lightsList, const PBRCamera& camera, int samplesPerPixel)
{
    std::vector<fungt::Vec3> framebuffer;
    framebuffer.resize(width * height);
    float aspectRatio = float(width) / float(height);

    PBRCamera cam(aspectRatio);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            fungt::Vec3 pixelColor(0.0f, 0.0f, 0.0f);


            for (int s = 0; s < samplesPerPixel; s++) {
                float u = (j + randomFloat()) / (width - 1);
                float v = (i + randomFloat()) / (height - 1);

                fungt::Ray ray = cam.getRay(u, v);

                HitData hit_data;
                bool isHit = false;
                float closest = FLT_MAX;

                for (const auto& tri : triangleList) {
                    HitData tempData;
                    if (Intersection::MollerTrumbore(ray, tri, 0.001f, closest, tempData)) {
                        isHit = true;
                        closest = tempData.dis;
                        hit_data = tempData;
                    }
                }

                if (isHit)
                    pixelColor += shadeNormal(hit_data.normal);
                else
                    pixelColor += fungt::Vec3(0.5f, 0.5f, 0.5f); // background
            }

            // Average and gamma correct
            pixelColor = pixelColor / float(samplesPerPixel);
            pixelColor = fungt::Vec3(std::sqrt(pixelColor.x),
                std::sqrt(pixelColor.y),
                std::sqrt(pixelColor.z)); // gamma 2.0

            framebuffer[i * width + j] = pixelColor;

        }

    }
    return framebuffer;
}