#include "../include/cuda_renderer.hpp"
__device__  fungt::Vec3 shadeNormal(const fungt::Vec3& normal) {
    // Convert from [-1,1] to [0,1]
    //return 0.5f * (normal + fungt::Vec3(1.0f, 1.0f, 1.0f));
    float intensity = std::abs(normal.dot(fungt::Vec3(0, 0, 1))); // dot with light direction
    //return fungt::Vec3(0.2f, 0.2f, 0.2f) + 0.6f * intensity; // gray + simple diffuse
    return fungt::Vec3(0.2f + 0.6f * intensity,
        0.2f + 0.6f * intensity,
        0.2f + 0.6f * intensity);

}
__device__  float randomFloat(curandState* state) {
    return curand_uniform(state);
}
__global__ void render_kernel(
    fungt::Vec3* framebuffer,
    const Triangle* triangles,
    int numOfTriangles,
    int width,
    int height,
    PBRCamera cam,
    int samplesPerPixel,
    int seed
) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= width || y >= height) return;
    int idx = y * width + x;



    curandState randomState;
    curand_init(seed + idx, 0, 0, &randomState);

    fungt::Vec3 pixelColor(0.0f, 0.0f, 0.0f);

    for (int s = 0; s < samplesPerPixel; s++) {
        float u = (x + randomFloat(&randomState)) / (width - 1);
        float v = (y + randomFloat(&randomState)) / (height - 1);

        fungt::Ray ray = cam.getRay(u, v);
        HitData hit_data;
        bool isHit = false;
        float closest = FLT_MAX;

        for (int i = 0; i < numOfTriangles; i++) {
            HitData tempData;
            if (Intersection::MollerTrumbore(ray, triangles[i], 0.001f, closest, tempData)) {
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

    pixelColor = pixelColor / float(samplesPerPixel);
    pixelColor = fungt::Vec3(sqrtf(pixelColor.x), sqrtf(pixelColor.y), sqrtf(pixelColor.z));
    framebuffer[idx] = pixelColor;
}
std::vector<fungt::Vec3>  CUDA_Renderer::RenderScene(
    int width, int height,
    const std::vector<Triangle>& triangleList,
    const PBRCamera& camera,
    int samplesPerPixel
) {
    std::vector<fungt::Vec3> framebuffer;
    const int imageSize = width * height;
    framebuffer.resize(imageSize);
    float aspectRatio = float(width) / float(height);
    unsigned int block_x = 16;
    unsigned int block_y = 16;

    dim3 block(block_x, block_y);
    unsigned int gridx = (width + block_x - 1) / block_x;
    unsigned int gridy = (height + block_y - 1) / block_y;
    std::cout << "Grid dimensions : (" << gridx << " , " << gridy << ")" << std::endl;
    dim3 grid(gridx, gridy);

    Triangle* device_Tlist = nullptr;
    size_t TlistSize = triangleList.size() * sizeof(Triangle);
    CUDA_CHECK(cudaMalloc(&device_Tlist, TlistSize));
    CUDA_CHECK(cudaMemcpy(device_Tlist, triangleList.data(), TlistSize, cudaMemcpyHostToDevice));

    //Final image buffer:

    fungt::Vec3* device_buff = nullptr;

    CUDA_CHECK(cudaMalloc(&device_buff, imageSize * sizeof(fungt::Vec3)));
    CUDA_CHECK(cudaMemset(device_buff, 0, imageSize * sizeof(fungt::Vec3))); //Fill with 0s
    int seed = 1337;
    render_kernel << <grid, block >> > (
        device_buff,
        device_Tlist,
        int(triangleList.size()),
        width,
        height,
        camera,
        samplesPerPixel,
        seed
        );


    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    CUDA_CHECK(cudaMemcpy(framebuffer.data(), device_buff, imageSize * sizeof(fungt::Vec3), cudaMemcpyDeviceToHost));
    CUDA_CHECK(cudaFree(device_buff));
    CUDA_CHECK(cudaFree(device_Tlist));

    return framebuffer;

}