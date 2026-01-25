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
fgt_device fungt::Vec3 skyColor(const fungt::Ray& ray) {
    float t = 0.5f * (ray.m_dir.y + 1.0f);
    //return (1.0f - t) * fungt::Vec3(1.0f, 1.0f, 1.0f) + t * fungt::Vec3(0.5f, 0.7f, 1.0f)*3.0f;
    return fungt::Vec3(0.0f, 0.0f, 0.0f); // Bright blu
    //return (t * fungt::Vec3(2.0f, 2.0f, 2.0f) + (1.0f - t) * fungt::Vec3(0.3f, 0.5f, 1.0f));

    // float t = 0.5f * (ray.m_dir.y + 1.0f);
    // fungt::Vec3 bottomColor(0.03f, 0.03f, 0.03f);  // Dark neutral gray
    // fungt::Vec3 topColor(0.1f, 0.1f, 0.1f);        // Medium gray
    // return (1.0f - t) * bottomColor + t * topColor;

    // // Deep space with subtle blue tint
    // float t = 0.5f * (ray.m_dir.y + 1.0f);
    // fungt::Vec3 bottomColor(0.01f, 0.01f, 0.02f);  // Very dark blue-black
    // fungt::Vec3 topColor(0.05f, 0.08f, 0.12f);     // Slightly lighter dark blue
    // return (1.0f - t) * bottomColor + t * topColor;

    // float t = 0.5f * (ray.m_dir.y + 1.0f);
    // fungt::Vec3 bottomColor(0.02f, 0.015f, 0.01f);  // Dark warm brown
    // fungt::Vec3 topColor(0.08f, 0.06f, 0.05f);      // Lighter warm gray
    // return (1.0f - t) * bottomColor + t * topColor;
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
fgt_device_gpu fungt::Vec3 pathTracer_CookTorrance(
    const fungt::Ray& initialRay,
    const Triangle* tris,
    const BVHNode *nodes,
    const Light* lights,
    cudaTextureObject_t* textures,
    int numOfTextures,  
    int numOfTriangles,
    int numOfNodes,
    int numOfLights,
    curandState* rng,
    fungt::RNG &fgtRng)
{
    fungt::Vec3 throughput(1.0f, 1.0f, 1.0f);
    fungt::Vec3 radiance(0.0f, 0.0f, 0.0f);
    fungt::Ray currRay = initialRay;

    for (int bounce = 0; bounce < 6; ++bounce) {
        HitData hit;
        //bool hitAny = traceRay(currRay, tris, numOfTriangles,textures, hit);
        bool hitAny = traceRayBVH(currRay,tris,nodes,numOfNodes,textures,hit);

        if (!hitAny) {
            radiance += throughput * skyColor(currRay);
            break;
        }

        fungt::Vec3 N = hit.normal.normalize();
        fungt::Vec3 V = (currRay.m_dir * (-1.0f)).normalize();

        // Extract material properties
        fungt::Vec3 baseColor = fungt::Vec3(hit.material.baseColor[0],
            hit.material.baseColor[1],
            hit.material.baseColor[2]);
        float metallic = fmaxf(0.0f, fminf(hit.material.metallic, 1.0f));
        float roughness = fmaxf(0.05f, fminf(hit.material.roughness, 1.0f));
        fungt::Vec3 dielectricF0 = fungt::Vec3(hit.material.reflectance,
            hit.material.reflectance,
            hit.material.reflectance);
        fungt::Vec3 F0 = lerp(dielectricF0, baseColor, metallic);

        // Add emission if any
        if (hit.material.emission > 0.0f) {
            radiance += throughput * baseColor * hit.material.emission;
        }

        // Direct lighting from all lights
        fungt::Vec3 directLight(0.0f);
        for (int l = 0; l < numOfLights; ++l) {
            fungt::Vec3 toLight = lights[l].m_pos - hit.point;
            float dist = toLight.length();
            fungt::Vec3 L = toLight / dist;

            // Shadow test
            fungt::Ray shadowRay(hit.point + hit.geometricNormal * 0.001f, L);
            HitData temp;
            //bool occluded = traceRay(shadowRay, tris, numOfTriangles,textures, temp) && temp.dis < dist;
            bool occluded = traceRayBVH(shadowRay, tris, nodes, numOfNodes, textures, temp) && temp.dis < dist;
            if (occluded) continue;

            // Light intensity with inverse square falloff
            fungt::Vec3 lightRadiance = lights[l].m_intensity / (dist * dist + 1e-6f);

            // Evaluate BRDF
            directLight += evaluateCookTorrance(N, V, L, hit.material, lightRadiance);
        }

        radiance += throughput * directLight;

        // Prepare indirect bounce - sample diffuse hemisphere
        fungt::Vec3 newDir = sampleHemisphere(N,fgtRng);
        //fungt::Vec3 newDir = sampleHemisphere(N, rng);
        //float cosTheta = fmaxf(newDir.dot(N), 0.0f);

        // Update throughput for next bounce
        // kD is the diffuse component (energy NOT reflected by Fresnel)
        fungt::Vec3 avgF = F_Schlick(F0, fmaxf(V.dot(N), 0.0f));
        fungt::Vec3 kD = (fungt::Vec3(1.0f, 1.0f, 1.0f) - avgF) * (1.0f - metallic);

        // For diffuse sampling: BRDF = kD * baseColor / PI
        // PDF = cosTheta / PI
        // throughput *= BRDF * cosTheta / PDF = (kD * baseColor / PI) * cosTheta / (cosTheta / PI)
        // Simplifies to: throughput *= kD * baseColor
        throughput = throughput * (kD * baseColor);

        currRay = fungt::Ray(hit.point + N * 0.001f, newDir);

        // Russian roulette termination
        if (bounce > 2) {
            float maxComponent = fmaxf(throughput.x, fmaxf(throughput.y, throughput.z));
            float p = fminf(0.95f, maxComponent);
            //if (randomFloat(rng) > p) break;
            if (fgtRng.nextFloat() > p) break;
            throughput = throughput / p;
        }
    }

    return radiance;
}

fgt_global void render_kernel(
    fungt::Vec3* framebuffer,
    const Triangle* triangles,
    const BVHNode * nodes,
    const Light *lights,
    cudaTextureObject_t* textures,
    int numTextures,
    int numOfTriangles,
    int numOfNodes,
    int numOfLights,
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

    curandState randomState;
    curand_init(seed + idx, 0, 0, &randomState);

    //fungt::Vec3 pixelColor(0.0f, 0.0f, 0.0f);

    // for (int s = 0; s < samplesPerPixel; s++) {
    //     float u = (x + randomFloat(&randomState)) / (width - 1);
    //     float v = (y + randomFloat(&randomState)) / (height - 1);

    //     fungt::Ray ray = cam.getRay(u, v);
    //     HitData hit_data;
    //     bool isHit = false;
    //     float closest = FLT_MAX;

    //     for (int i = 0; i < numOfTriangles; i++) {
    //         HitData tempData;
    //         if (Intersection::MollerTrumbore(ray, triangles[i], 0.001f, closest, tempData)) {
    //             isHit = true;
    //             closest = tempData.dis;
    //             hit_data = tempData;
    //         }
    //     }

    //     if (isHit)
    //         pixelColor += shadeNormal(hit_data.normal);
    //     else
    //         pixelColor += fungt::Vec3(0.5f, 0.5f, 0.5f); // background

    // }

    // pixelColor = pixelColor / float(samplesPerPixel);
    // pixelColor = fungt::Vec3(sqrtf(pixelColor.x), sqrtf(pixelColor.y), sqrtf(pixelColor.z));
    // framebuffer[idx] = pixelColor;

    fungt::Vec3 pixel(0.0f);
    for (int s = 0; s < samplesPerPixel; s++) {
        //float u = (x + randomFloat(&randomState)) / (width - 1);
        //float v = (y + randomFloat(&randomState)) / (height - 1);
        float u = (x + rng.nextFloat()) / (width - 1);
        float v = (y + rng.nextFloat()) / (height - 1);
        fungt::Ray ray = cam.getRay(u, v);

        pixel += pathTracer_CookTorrance(ray, triangles,nodes, lights,
                                        textures,numTextures, numOfTriangles,
                                        numOfNodes, numOfLights, &randomState,rng);
    }

    pixel = pixel / float(samplesPerPixel);
    framebuffer[idx] = fungt::Vec3(pixel.x, pixel.y, pixel.z);



}
std::vector<fungt::Vec3>  CUDA_Renderer::RenderScene(
    int width, int height,
    const std::vector<Triangle>& triangleList,
    const std::vector<BVHNode> &nodes,
    const std::vector<Light> &lightsList,
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
        m_textureObj,
        m_numTextures,
        int(triangleList.size()),
        int(nodes.size()),
        int(lightsList.size()),
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
    CUDA_CHECK(cudaFree(device_bvhNode));
    CUDA_CHECK(cudaFree(device_lights));

    return framebuffer;

}