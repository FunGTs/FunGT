#include "PBR/Render/include/cuda_renderer.hpp"
#include "PBR/Render/brdf/cook_torrance.hpp"
#include "PBR/PBRCamera/pbr_camera.hpp"
fgt_device_gpu  float randomFloat(curandState* state) {
    return curand_uniform(state);
}
fgt_device_gpu bool traceRay(
    const fungt::Ray& ray, 
    const Triangle* tris, 
    int numOFTriangles, 
    cudaTextureObject_t* textures,
    HitData& hit) {

    bool hitSomething = false;
    float closest = FLT_MAX;
    for (int i = 0; i < numOFTriangles; i++) {
        HitData temp;
        if (Intersection::MollerTrumbore(ray, tris[i], 0.001f, closest, temp)) {
            hitSomething = true;
            closest = temp.dis;
            hit = temp;
            // Calculate geometric normal (for ray offset)
            fungt::Vec3 e1 = tris[i].v1 - tris[i].v0;
            fungt::Vec3 e2 = tris[i].v2 - tris[i].v0;
            hit.geometricNormal = e1.cross(e2).normalize();

            // Interpolate shading normal (for lighting - SMOOTH SHADING!)
            hit.normal = (tris[i].n0 * temp.bary.x +
                tris[i].n1 * temp.bary.y +
                tris[i].n2 * temp.bary.z).normalize();
            // Make sure shading normal faces same hemisphere as geometric normal
            if (hit.normal.dot(hit.geometricNormal) < 0.0f) {
                hit.normal = hit.normal * -1.0f;
            }
            hit.material = tris[i].material; // store material directly
            if (hit.material.baseColorTexIdx >= 0 && textures != nullptr) {
                // Interpolate UVs using barycentric coordinates
                float u = tris[i].uvs[0][0] * temp.bary.x +
                    tris[i].uvs[1][0] * temp.bary.y +
                    tris[i].uvs[2][0] * temp.bary.z;
                float v = tris[i].uvs[0][1] * temp.bary.x +
                    tris[i].uvs[1][1] * temp.bary.y +
                    tris[i].uvs[2][1] * temp.bary.z;

                // Sample CUDA texture
                float4 texColor = tex2D<float4>(textures[hit.material.baseColorTexIdx], u, v);

                // Override base color with texture color
                hit.material.baseColor[0] = texColor.x;
                hit.material.baseColor[1] = texColor.y;
                hit.material.baseColor[2] = texColor.z;
            }
        }   
    }
    return hitSomething;
}

fgt_device_gpu fungt::Vec3 sampleHemisphere(const fungt::Vec3& normal, curandState* state) {
    float u = randomFloat(state);
    float v = randomFloat(state);
    float theta = acosf(sqrtf(1.0f - u));
    float phi = 2.0f * M_PI * v;

    float xs = sinf(theta) * cosf(phi);
    float ys = sinf(theta) * sinf(phi);
    float zs = cosf(theta);

    // Transform to world space using normal
    fungt::Vec3 tangent = fabs(normal.x) > 0.1f ? fungt::Vec3(0, 1, 0).cross(normal).normalize()
        : fungt::Vec3(1, 0, 0).cross(normal).normalize();
    fungt::Vec3 bitangent = normal.cross(tangent);
    return (tangent * xs + bitangent * ys + normal * zs).normalize();


}

fgt_device_gpu fungt::Vec3 pathTracer(const fungt::Ray& initialRay, const Triangle* tris,const Light *lights, int numOfTriangles,int numOfLights, curandState* rng) {
    fungt::Vec3 color(1.0f,1.0f,1.0f);
    fungt::Vec3 accumulated(0.0f, 0.f,0.f);


    fungt::Ray currRay = initialRay;
    
    for(int bounce = 0; bounce<4; bounce++){
        HitData hit;
        bool hitAny = traceRay(currRay, tris, numOfTriangles,nullptr, hit);

        if (!hitAny) {
            accumulated = accumulated + color * skyColor(currRay);
            break;
        }
        fungt::Vec3 hitColor(0.0f);
        for(int l = 0; l<numOfLights; l++){
            fungt::Vec3 toLight = lights[l].m_pos - hit.point;
            float lightDist = toLight.length();
            fungt::Vec3 lightDir = toLight / lightDist;


            // Shadow ray
            fungt::Ray shadowRay(hit.point + hit.normal * 0.001f, lightDir);

            HitData shadowHit;
            bool occluded = traceRay(shadowRay, tris, numOfTriangles,nullptr, shadowHit) && shadowHit.dis < lightDist;

            if (!occluded) {
                float NdotL = fmaxf(hit.normal.dot(lightDir), 0.0f);
                //fungt::Vec3 albedo(tris->material.baseColor[0], tris->material.baseColor[1], tris->material.baseColor[2]);
                //fungt::Vec3 albedo = fungt::toFungtVec3(hit.material.diffuse);
                fungt::Vec3 albedo(hit.material.baseColor[0],
                    hit.material.baseColor[1],
                    hit.material.baseColor[2]);
                hitColor += albedo * lights[l].m_intensity * NdotL / (lightDist * lightDist);
            }
        }

        //accumulated = accumulated + color * skyColor(currRay);
        accumulated += color * hitColor;
       
        // Material-based diffuse color
        //fungt::Vec3 albedo = fungt::toFungtVec3(hit.material.diffuse);
        //fungt::Vec3 albedo(0.8, 0.8, 0.8);
        // Diffuse bounce
        fungt::Vec3 albedo(hit.material.baseColor[0],
            hit.material.baseColor[1],
            hit.material.baseColor[2]);
        fungt::Vec3 newDir = sampleHemisphere(hit.normal, rng);
        currRay = fungt::Ray(hit.point + hit.normal * 0.001f, newDir);

        color = color*albedo;

        // Russian roulette for termination
        if (bounce > 2) {
            float p = 0.8f;
            if (randomFloat(rng) > p) break;
            color = color / p;
        }

    }
    return accumulated;

}
fgt_device fungt::Vec3 shadeNormal(const fungt::Vec3& normal) {
    // Convert from [-1,1] to [0,1]
    //return 0.5f * (normal + fungt::Vec3(1.0f, 1.0f, 1.0f));
    float intensity = std::abs(normal.dot(fungt::Vec3(0, 0, 1))); // dot with light direction
    //return fungt::Vec3(0.2f, 0.2f, 0.2f) + 0.6f * intensity; // gray + simple diffuse
    return fungt::Vec3(0.2f + 0.6f * intensity,
        0.2f + 0.6f * intensity,
        0.2f + 0.6f * intensity);

}
fgt_global void render_kernel(
    fungt::Vec3* framebuffer,
    const Triangle* triangles,
    const BVHNode * nodes,
    const Light *lights,
    const int *emissiveTris,
    cudaTextureObject_t* textures,
    int numTextures,
    int numOfTriangles,
    int numOfNodes,
    int numOfLights,
    int numOfEmissiveTris,
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

    fungt::RNG rng(idx * 1337ULL + 123ULL);

    fungt::Vec3 pixel(0.0f);
    for (int s = 0; s < samplesPerPixel; s++) {
        //float u = (x + randomFloat(&randomState)) / (width - 1);
        //float v = (y + randomFloat(&randomState)) / (height - 1);
        float u = (x + rng.nextFloat()) / (width - 1);
        float v = (y + rng.nextFloat()) / (height - 1);
        fungt::Ray ray = cam.getRay(u, v);

        pixel += pathTracer_CookTorrance(ray, triangles,nodes, lights, emissiveTris,
                                        textures,numTextures, numOfTriangles,
                                        numOfNodes, numOfLights,numOfEmissiveTris,rng);
    }

    pixel = pixel / float(samplesPerPixel);
    framebuffer[idx] = fungt::Vec3(pixel.x, pixel.y, pixel.z);

}
std::vector<fungt::Vec3>  CUDA_Renderer::RenderScene(
    int width, int height,
    const std::vector<Triangle>& triangleList,
    const std::vector<BVHNode> &nodes,
    const std::vector<Light> &lightsList,
    const std::vector<int>& emissiveTriIndices,
    const PBRCamera& camera,
    int samplesPerPixel,
    int sampleOffset
) {
    prepareTextures();

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

    int* device_emissiveTris = nullptr;
    int numEmissiveTris = emissiveTriIndices.size();
    if (numEmissiveTris > 0) {
        size_t emissiveLightsSize = numEmissiveTris*sizeof(int);
        CUDA_CHECK(cudaMalloc(&device_emissiveTris, emissiveLightsSize));
        CUDA_CHECK(cudaMemcpy(device_emissiveTris, emissiveTriIndices.data(), emissiveLightsSize, cudaMemcpyHostToDevice));
    }

    Triangle* device_Tlist = nullptr;
    size_t TlistSize = triangleList.size() * sizeof(Triangle);
    CUDA_CHECK(cudaMalloc(&device_Tlist, TlistSize));
    CUDA_CHECK(cudaMemcpy(device_Tlist, triangleList.data(), TlistSize, cudaMemcpyHostToDevice));

    BVHNode* device_bvhNode = nullptr;
    size_t BvhNodeSize = nodes.size()*sizeof(BVHNode);
    CUDA_CHECK(cudaMalloc(&device_bvhNode,BvhNodeSize));
    CUDA_CHECK(cudaMemcpy(device_bvhNode,nodes.data(),BvhNodeSize,cudaMemcpyHostToDevice));

    Light *device_lights = nullptr;
    size_t LlistSize = lightsList.size()*sizeof(Light);
    CUDA_CHECK(cudaMalloc(&device_lights, LlistSize));
    CUDA_CHECK(cudaMemcpy(device_lights, lightsList.data(), LlistSize, cudaMemcpyHostToDevice));
    
    //Final image buffer:

    fungt::Vec3* device_buff = nullptr;

    CUDA_CHECK(cudaMalloc(&device_buff, imageSize * sizeof(fungt::Vec3)));
    CUDA_CHECK(cudaMemset(device_buff, 0, imageSize * sizeof(fungt::Vec3))); //Fill with 0s
    int seed = 1337;

    //Check for texture
    if(m_textureObj){
        std::cout<<"Using CUDA Textures"<<std::endl;
    }
    else{
        std::cout << "WARNING: CUDA Textures ptr is NUL " << std::endl;
    }
    

    render_kernel << <grid, block >> > (
        device_buff,
        device_Tlist,
        device_bvhNode,
        device_lights,
        device_emissiveTris,
        m_textureObj,
        m_numTextures,
        int(triangleList.size()),
        int(nodes.size()),
        int(lightsList.size()),
        numEmissiveTris,
        width,
        height,
        camera,
        samplesPerPixel,
        seed
    );


    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    //Get buffer data
    CUDA_CHECK(cudaMemcpy(framebuffer.data(), device_buff, imageSize * sizeof(fungt::Vec3), cudaMemcpyDeviceToHost));

    if (device_emissiveTris) {
        CUDA_CHECK(cudaFree(device_emissiveTris));
    }
    CUDA_CHECK(cudaFree(device_buff));
    CUDA_CHECK(cudaFree(device_Tlist));
    CUDA_CHECK(cudaFree(device_bvhNode));
    CUDA_CHECK(cudaFree(device_lights));

    return framebuffer;

}
