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

Space::Space(std::vector<Triangle>& triangleList)
{
    Material gray(glm::vec3(0.2f), glm::vec3(0.8f), glm::vec3(0.8f), 32.0f, "DefaultGray");
    m_triangles = std::move(triangleList);
    m_lights.push_back(Light(
        fungt::Vec3(2.0f, 2.0f, 2.0f),    // position
        fungt::Vec3(10.0f, 10.0f, 10.0f)  // strong white intensity
    ));
}


Space::~Space(){
}

std::vector<fungt::Vec3> Space::Render(const int width, const int height) {
    switch (ComputeRender::GetBackend())
  {
    case Compute::Backend::CPU:
    {
        /* code */
        std::cout << "Using CPU to render scene" << std::endl;
        m_computeRenderer = std::make_unique<CPU_Renderer>();
        break;
    }
#ifdef FUNGT_USE_CUDA
    case Compute::Backend::CUDA:
    {
        /* code */
        std::cout << "Using CUDA to render scene" << std::endl;
        m_computeRenderer = std::make_unique<CUDA_Renderer>();
        break;
    }
#endif
  default:
      throw std::runtime_error("Unknown Compute API!");
  }
  //Starting render:
    std::cout << "Starting render" << std::endl;
    std::vector<fungt::Vec3> frameBuffer = m_computeRenderer->RenderScene(width, height,m_triangles,m_lights,m_camera,m_samplesPerPixel);

    return frameBuffer;
}

void Space::SaveFrameBufferAsPNG(const std::vector<fungt::Vec3>& framebuffer, int width, int height)
{
    std::vector<unsigned char> pixels(width * height * 3);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Flip Y coordinate
            int srcIdx = y * width + x;              // Original index
            int dstIdx = (height - 1 - y) * width + x;  // Flipped index

            fungt::Vec3 color = framebuffer[srcIdx];

            // Clamp and gamma correct
            color.x = std::pow(std::clamp(color.x, 0.0f, 1.0f), 1.0f / 2.2f);
            color.y = std::pow(std::clamp(color.y, 0.0f, 1.0f), 1.0f / 2.2f);
            color.z = std::pow(std::clamp(color.z, 0.0f, 1.0f), 1.0f / 2.2f);

            pixels[dstIdx * 3 + 0] = static_cast<unsigned char>(255.99f * color.x);
            pixels[dstIdx * 3 + 1] = static_cast<unsigned char>(255.99f * color.y);
            pixels[dstIdx * 3 + 2] = static_cast<unsigned char>(255.99f * color.z);
        }
    }

    std::string file_name = ComputeRender::GetBackendName() + "_output.png";
    stbi_write_png(file_name.c_str(), width, height, 3, pixels.data(), width * 3);
}

void Space::setSamples(int numOfSamples)
{
    m_samplesPerPixel = numOfSamples;
}



