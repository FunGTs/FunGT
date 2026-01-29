
#include "sycl_renderer.hpp"
fgt_device inline fungt::Vec3 skyColor(const fungt::Ray& ray) {
    return fungt::Vec3(0.4, 0.4f, 0.4);
}


fgt_device_gpu fungt::Vec3 pathTracer_CookTorrance(
    const fungt::Ray& initialRay,
    const Triangle* tris,
    const BVHNode* nodes,
    const Light* lights,
    const int* emissiveTris,
    const syclexp::sampled_image_handle* textures,
    int numTextures,
    int numOfTriangles,
    int numOfNodes,
    int numOfLights,
    int numEmissiveTris,
    fungt::RNG& rng)
{
    fungt::Vec3 throughput(1.0f, 1.0f, 1.0f);
    fungt::Vec3 radiance(0.0f, 0.0f, 0.0f);
    fungt::Ray currRay = initialRay;

    for (int bounce = 0; bounce < 6; ++bounce) {
        HitData hit;
        bool hitAny = traceRayBVH(currRay, tris, nodes, numOfNodes, textures, hit);

        if (!hitAny) {
            radiance += throughput * skyColor(currRay);
            break;
        }

        fungt::Vec3 N = hit.normal.normalize();
        fungt::Vec3 V = (currRay.m_dir * (-1.0f)).normalize();

        fungt::Vec3 baseColor = fungt::Vec3(
            hit.material.baseColor[0],
            hit.material.baseColor[1],
            hit.material.baseColor[2]);

        float metallic = fmaxf(0.0f, fminf(hit.material.metallic, 1.0f));
        float roughness = fmaxf(0.05f, fminf(hit.material.roughness, 1.0f));

        fungt::Vec3 dielectricF0 = fungt::Vec3(
            hit.material.reflectance,
            hit.material.reflectance,
            hit.material.reflectance);
        fungt::Vec3 F0 = lerp(dielectricF0, baseColor, metallic);

        if (hit.material.emission > 0.0f) {
            radiance += throughput * baseColor * hit.material.emission;
        }

        fungt::Vec3 directLight(0.0f);
        for (int l = 0; l < numOfLights; ++l) {
            fungt::Vec3 toLight = lights[l].m_pos - hit.point;
            float dist = toLight.length();
            fungt::Vec3 L = toLight / dist;

            fungt::Ray shadowRay(hit.point + hit.geometricNormal * 0.001f, L);
            HitData temp;
            bool occluded = traceRayBVH(shadowRay, tris, nodes, numOfNodes, textures, temp) && temp.dis < dist;

            if (occluded) continue;

            fungt::Vec3 lightRadiance = lights[l].m_intensity / (dist * dist + 1e-6f);
            directLight += evaluateCookTorrance(N, V, L, hit.material, lightRadiance);
        }

        radiance += throughput * directLight;

        //Emissive Triangles


        if (numEmissiveTris > 0) {
            fungt::Vec3 lightPos, lightNormal, lightEmission;
            float lightPdf;

            sampleEmissiveLight(tris, emissiveTris, numEmissiveTris, rng,
                lightPos, lightNormal, lightEmission, lightPdf);

            if (lightPdf > 0.0f) {
                fungt::Vec3 toLight = lightPos - hit.point;
                float distToLight = toLight.length();
                fungt::Vec3 L = toLight / distToLight;

                // Shadow ray to check visibility
                fungt::Ray shadowRay(hit.point + hit.geometricNormal * 0.001f, L);
                HitData shadowHit;
                bool visible = !traceRayBVH(shadowRay, tris, nodes, numOfNodes, textures, shadowHit) ||
                    shadowHit.dis > (distToLight - 0.001f);

                if (visible) {
                    float cosTheta = fmaxf(0.0f, N.dot(L));
                    float cosLight = fmaxf(0.0f, lightNormal.dot(L * -1.0f));

                    if (cosTheta > 0.0f && cosLight > 0.0f) {
                        // Evaluate Cook-Torrance BRDF for this light direction
                        fungt::Vec3 emissiveLight = lightEmission / (distToLight * distToLight + 1e-6f);
                        fungt::Vec3 neeContribution = evaluateCookTorrance(N, V, L, hit.material, emissiveLight);

                        // Geometric term for area light
                        float geometryTerm = cosLight / lightPdf;

                        radiance += throughput * neeContribution * geometryTerm;
                    }
                }
            }
        }

        fungt::Vec3 newDir = sampleHemisphere(N, rng);

        fungt::Vec3 avgF = F_Schlick(F0, fmaxf(V.dot(N), 0.0f));
        fungt::Vec3 kD = (fungt::Vec3(1.0f, 1.0f, 1.0f) - avgF) * (1.0f - metallic);
        throughput = throughput * (kD * baseColor);

        currRay = fungt::Ray(hit.point + N * 0.001f, newDir);

        if (bounce > 2) {
            float maxComponent = fmaxf(throughput.x, fmaxf(throughput.y, throughput.z));
            float p = fminf(0.95f, maxComponent);
            if (rng.nextFloat() > p) break;
            throughput = throughput / p;
        }
    }

    return radiance;
}
std::vector<fungt::Vec3> SYCL_Renderer::RenderScene(
    int width,
    int height,
    const std::vector<Triangle>& triangleList,
    const std::vector<BVHNode>& nodes,
    const std::vector<Light>& lightsList,
    const std::vector<int>& emissiveTriIndices,
    const PBRCamera& camera,
    int samplesPerPixel)
{
    int imageSize = width * height;
    std::vector<fungt::Vec3> framebuffer(imageSize);
    std::cout << "SYCL_Renderer: Rendering " << width << "x" << height
        << " with " << samplesPerPixel << " samples" << std::endl;

    // USM allocations

    int* dev_emissiveTris = nullptr;
    int numEmissiveTris = emissiveTriIndices.size();
    if (numEmissiveTris > 0) {
        dev_emissiveTris = sycl::malloc_device<int>(numEmissiveTris, m_queue);
        m_queue.memcpy(dev_emissiveTris, emissiveTriIndices.data(), numEmissiveTris * sizeof(int));
    }

    Triangle* dev_triList = sycl::malloc_device<Triangle>(triangleList.size(), m_queue);
    BVHNode* dev_bvhNode = sycl::malloc_device<BVHNode>(nodes.size(), m_queue);
    Light* dev_lights = sycl::malloc_device<Light>(lightsList.size(), m_queue);
    fungt::Vec3* dev_buff = sycl::malloc_device<fungt::Vec3>(imageSize, m_queue);

    m_queue.memcpy(dev_triList, triangleList.data(), triangleList.size() * sizeof(Triangle));
    m_queue.memcpy(dev_bvhNode, nodes.data(), nodes.size() * sizeof(BVHNode));
    m_queue.memcpy(dev_lights, lightsList.data(), lightsList.size() * sizeof(Light));
    m_queue.wait();

    int numTriangles = triangleList.size();
    int numNodes = nodes.size();
    int numLights = lightsList.size();
    int numTextures = m_numTextures;
    auto textureHandles = m_textureHandles;

    // Tiled rendering
    const int tileSize = 64;  // Adjust if still too heavy (try 32) or too slow (try 128)

    for (int tileY = 0; tileY < height; tileY += tileSize) {
        for (int tileX = 0; tileX < width; tileX += tileSize) {
            int tileW = std::min(tileSize, width - tileX);
            int tileH = std::min(tileSize, height - tileY);

            m_queue.submit([&](sycl::handler& h) {
                h.parallel_for(
                    sycl::range<2>{static_cast<size_t>(tileW), static_cast<size_t>(tileH)},
                    [=](sycl::id<2> idx) {
                        int x = tileX + idx[0];
                        int y = tileY + idx[1];
                        int pixelIdx = x + y * width;

                        fungt::RNG rng(pixelIdx * 1337ULL + 123ULL);

                        fungt::Vec3 pixel(0.0f);
                        for (int s = 0; s < samplesPerPixel; s++) {
                            float u = (x + rng.nextFloat()) / (width - 1);
                            float v = (y + rng.nextFloat()) / (height - 1);

                            fungt::Ray ray = camera.getRay(u, v);

                            pixel += pathTracer_CookTorrance(
                                ray,
                                dev_triList,
                                dev_bvhNode,
                                dev_lights,
                                dev_emissiveTris,
                                textureHandles,
                                numTextures,
                                numTriangles,
                                numNodes,
                                numLights,
                                numEmissiveTris,
                                rng);
                        }

                        pixel = pixel / float(samplesPerPixel);
                        dev_buff[pixelIdx] = pixel;
                    });
                }).wait();  // Wait after each tile - lets system breathe
        }
    }

    m_queue.memcpy(framebuffer.data(), dev_buff, imageSize * sizeof(fungt::Vec3));
    m_queue.wait();
    if (dev_emissiveTris) {
        sycl::free(dev_emissiveTris, m_queue);
    }
    sycl::free(dev_triList, m_queue);
    sycl::free(dev_bvhNode, m_queue);
    sycl::free(dev_lights, m_queue);
    sycl::free(dev_buff, m_queue);

    return framebuffer;
}
void SYCL_Renderer::createQueue()
{
    try {
        
        //flib::sycl_handler::sys_info(); // Prints system info
        flib::sycl_handler::select_device("Intel"); // Selects a vendor for your computations
        flib::sycl_handler::get_device_info(); // Prints current device info

        m_queue = flib::sycl_handler::get_queue();

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

sycl::queue& SYCL_Renderer::getQueue()
{
    return m_queue;
}
