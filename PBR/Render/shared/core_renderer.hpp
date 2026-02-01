#if !defined(_CORE_RENDERER_H_)
#define _CORE_RENDERER_H_
#include "gpu/include/fgt_cpu_device.hpp"
#include "Triangle/triangle.hpp"
#include "Random/fgt_rng.hpp"
#include "PBR/Ray/ray.hpp"
#include "PBR/BVH/bvh_node.hpp"
#include "PBR/TextureManager/sampler2d_texture.hpp"
#include "PBR/HitData/hit_data.hpp"
#include "PBR/Intersection/intersection.hpp"
#include "PBR/Render/brdf/cook_torrance.hpp"
// Sample a random point on a random emissive triangle
fgt_device_gpu void sampleEmissiveLight(
    const Triangle* tris,
    const int* emissiveTris,
    int numEmissiveTris,
    fungt::RNG& rng,
    fungt::Vec3& lightPos,
    fungt::Vec3& lightNormal,
    fungt::Vec3& lightEmission,
    float& pdf)
{
    if (numEmissiveTris == 0) {
        pdf = 0.0f;
        return;
    }

    // Pick random emissive triangle
   // Pick random emissive triangle using integer RNG
    uint32_t randInt = rng.nextU32();
    int idx = randInt % numEmissiveTris;  // Modulo gives range [0, numEmissiveTris-1]
    int triIdx = emissiveTris[idx];
    const Triangle& tri = tris[triIdx];

    // Sample random point on triangle using barycentric coordinates
    float r1 = rng.nextFloat();
    float r2 = rng.nextFloat();
    if (r1 + r2 > 1.0f) {
        r1 = 1.0f - r1;
        r2 = 1.0f - r2;
    }
    float r3 = 1.0f - r1 - r2;

    lightPos = tri.v0 * r1 + tri.v1 * r2 + tri.v2 * r3;
    lightNormal = (tri.n0 * r1 + tri.n1 * r2 + tri.n2 * r3).normalize();
    lightEmission = tri.material.emission;

    // PDF = 1 / (numTriangles * triangleArea)
    fungt::Vec3 edge1 = tri.v1 - tri.v0;
    fungt::Vec3 edge2 = tri.v2 - tri.v0;
    float area = 0.5f * edge1.cross(edge2).length();
    if (area < 1e-8f) area = 1e-8f;
    pdf = 1.0f / (numEmissiveTris * area);
}
fgt_device_gpu inline fungt::Vec3 sampleHemisphere(const fungt::Vec3& normal, fungt::RNG& fgtRNG) {

    float u = fgtRNG.nextFloat();
    float v = fgtRNG.nextFloat();

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
// Shadow ray traversal - returns TRUE if anything blocks the ray
// Unlike traceRayBVH, this exits immediately on ANY hit (no closest hit needed)
fgt_device_gpu bool traceShadowRayBVH(
    const fungt::Ray& ray,
    const Triangle* tris,
    const BVHNode* bvhNodes,
    int numNodes,
    float maxDist)  // Only check hits closer than this (distance to light)
{
    int stack[64];
    int stackPtr = 0;
    stack[stackPtr++] = 0;  // Start with root

    while (stackPtr > 0) {
        int nodeIdx = stack[--stackPtr];
        const BVHNode& node = bvhNodes[nodeIdx];

        // Test AABB with maxDist as upper bound
        if (!Intersection::intersectAABB(ray, node.m_boundingBox, 0.001f, maxDist)) {
            continue;
        }

        if (node.isLeaf()) {
            // Test triangles - return immediately on ANY hit
            for (int i = 0; i < node.triCount; i++) {
                int triIdx = node.firstTriIdx + i;
                HitData temp;
                if (Intersection::MollerTrumbore(ray, tris[triIdx], 0.001f, maxDist, temp)) {
                    return true;  // BLOCKED - early exit
                }
            }
        }
        else {
            stack[stackPtr++] = node.leftChild;
            stack[stackPtr++] = node.rightChild;
        }
    }

    return false;  // Nothing blocked the ray
}
fgt_device_gpu bool inline traceRayBVH(
    const fungt::Ray& ray,
    const Triangle* tris,
    const BVHNode* bvhNodes,
    int numNodes,
    const TextureDeviceObject* textures,
    HitData& hit

){

    bool hitSomething = false;
    float closest = FLT_MAX;

    // Stack-based traversal (no recursion on GPU!)
    int stack[64];  // Stack to track nodes to visit
    int stackPtr = 0;
    stack[stackPtr++] = 0;  // Start with root node (index 0)

    while (stackPtr > 0) {
        // Pop node from stack
        int nodeIdx = stack[--stackPtr];
        const BVHNode& node = bvhNodes[nodeIdx];

        // Test ray against node's bounding box
        if (!Intersection::intersectAABB(ray, node.m_boundingBox, 0.001f, closest)) {
            continue;  // Miss! Skip this entire subtree
        }

        // Hit the box! Now check if it's a leaf or internal node
        if (node.isLeaf()) {
            // LEAF NODE: Test triangles
            for (int i = 0; i < node.triCount; i++) {
                int triIdx = node.firstTriIdx + i;
                HitData temp;

                if (Intersection::MollerTrumbore(ray, tris[triIdx], 0.001f, closest, temp)) {
                    hitSomething = true;
                    closest = temp.dis;
                    hit = temp;

                    // Calculate geometric normal
                    fungt::Vec3 e1 = tris[triIdx].v1 - tris[triIdx].v0;
                    fungt::Vec3 e2 = tris[triIdx].v2 - tris[triIdx].v0;
                    hit.geometricNormal = e1.cross(e2).normalize();

                    // Interpolate shading normal
                    hit.normal = (tris[triIdx].n0 * temp.bary.x +
                        tris[triIdx].n1 * temp.bary.y +
                        tris[triIdx].n2 * temp.bary.z).normalize();

                    // Ensure normal faces same hemisphere
                    if (hit.normal.dot(hit.geometricNormal) < 0.0f) {
                        hit.normal = hit.normal * -1.0f;
                    }

                    hit.material = tris[triIdx].material;

                    // Texture sampling (if applicable)
                    if (hit.material.baseColorTexIdx >= 0 && textures != nullptr) {
                        float u = tris[triIdx].uvs[0][0] * temp.bary.x +
                            tris[triIdx].uvs[1][0] * temp.bary.y +
                            tris[triIdx].uvs[2][0] * temp.bary.z;
                        float v = tris[triIdx].uvs[0][1] * temp.bary.x +
                            tris[triIdx].uvs[1][1] * temp.bary.y +
                            tris[triIdx].uvs[2][1] * temp.bary.z;

                        fungt::Vec3 texColor = sampleTexture2D(textures[hit.material.baseColorTexIdx], u, v);
                        texColor.x = powf(texColor.x, 2.2f);
                        texColor.y = powf(texColor.y, 2.2f);
                        texColor.z = powf(texColor.z, 2.2f);

                        hit.material.baseColor[0] = texColor.x;
                        hit.material.baseColor[1] = texColor.y;
                        hit.material.baseColor[2] = texColor.z;
                    }
                }
            }
        }
        else {
            // INTERNAL NODE: Push children onto stack
            // Push both children (they'll be tested in next iterations)
            stack[stackPtr++] = node.leftChild;
            stack[stackPtr++] = node.rightChild;
        }
    }

    return hitSomething;


}
fgt_device_gpu fungt::Vec3 pathTracer_CookTorrance(
    const fungt::Ray& initialRay,
    const Triangle* tris,
    const BVHNode* nodes,
    const Light* lights,
    const int* emissiveTris,
    const TextureDeviceObject* textures,
    int numOfTextures,
    int numOfTriangles,
    int numOfNodes,
    int numOfLights,
    int numOfEmissiveTris,
    fungt::RNG& fgtRng)
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

        // Emission
        if (hit.material.emission > 0.0f) {
            radiance += throughput * baseColor * hit.material.emission;
        }

        // Direct lighting from point lights
        fungt::Vec3 directLight(0.0f);
        for (int l = 0; l < numOfLights; ++l) {
            fungt::Vec3 toLight = lights[l].m_pos - hit.point;
            float dist = toLight.length();
            fungt::Vec3 L = toLight / dist;

            // OPTIMIZED: Early-exit shadow ray
            fungt::Ray shadowRay(hit.point + hit.geometricNormal * 0.001f, L);
            if (traceShadowRayBVH(shadowRay, tris, nodes, numOfNodes, dist)) {
                continue;  // Blocked
            }

            fungt::Vec3 lightRadiance = lights[l].m_intensity / (dist * dist + 1e-6f);
            directLight += evaluateCookTorrance(N, V, L, hit.material, lightRadiance);
        }
        radiance += throughput * directLight;

        // NEE for emissive triangles (only first 3 bounces)
        if (numOfEmissiveTris > 0 && bounce < 3) {
            fungt::Vec3 lightPos, lightNormal, lightEmission;
            float lightPdf;

            sampleEmissiveLight(tris, emissiveTris, numOfEmissiveTris, fgtRng,
                lightPos, lightNormal, lightEmission, lightPdf);

            if (lightPdf > 0.0f) {
                fungt::Vec3 toLight = lightPos - hit.point;
                float distToLight = toLight.length();
                fungt::Vec3 L = toLight / distToLight;

                float cosTheta = N.dot(L);
                float cosLight = lightNormal.dot(L * -1.0f);

                // Skip if facing away (before shooting shadow ray)
                if (cosTheta > 0.0f && cosLight > 0.0f) {

                    // OPTIMIZED: Early-exit shadow ray
                    fungt::Ray shadowRay(hit.point + hit.geometricNormal * 0.001f, L);
                    bool occluded = traceShadowRayBVH(shadowRay, tris, nodes, numOfNodes,
                        distToLight - 0.001f);

                    if (!occluded) {
                        fungt::Vec3 neeContribution = evaluateCookTorrance(N, V, L,
                            hit.material,
                            lightEmission);
                        float geometryTerm = cosLight / (distToLight * distToLight);
                        radiance += throughput * neeContribution * geometryTerm * cosTheta / lightPdf;
                    }
                }
            }
        }

        // Indirect bounce
        fungt::Vec3 newDir = sampleHemisphere(N, fgtRng);
        fungt::Vec3 avgF = F_Schlick(F0, fmaxf(V.dot(N), 0.0f));
        fungt::Vec3 kD = (fungt::Vec3(1.0f, 1.0f, 1.0f) - avgF) * (1.0f - metallic);
        throughput = throughput * (kD * baseColor);

        currRay = fungt::Ray(hit.point + N * 0.001f, newDir);

        // Russian roulette
        if (bounce > 2) {
            float maxComponent = fmaxf(throughput.x, fmaxf(throughput.y, throughput.z));
            float p = fminf(0.95f, maxComponent);
            if (fgtRng.nextFloat() > p) break;
            throughput = throughput / p;
        }
    }

    return radiance;
}
#endif // _CORE_RENDERER_H_
