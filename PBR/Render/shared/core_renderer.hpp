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
fgt_device_gpu fungt::Vec3 pathTracer_CookTorrance(
    const fungt::Ray& initialRay,
    const Triangle* tris,
    const BVHNode* nodes,
    const Light* lights,
    const TextureDeviceObject* textures,
    int numOfTextures,
    int numOfTriangles,
    int numOfNodes,
    int numOfLights,
    fungt::RNG& fgtRng)
{
    fungt::Vec3 throughput(1.0f, 1.0f, 1.0f);
    fungt::Vec3 radiance(0.0f, 0.0f, 0.0f);
    fungt::Ray currRay = initialRay;

    for (int bounce = 0; bounce < 6; ++bounce) {
        HitData hit;
        //bool hitAny = traceRay(currRay, tris, numOfTriangles,textures, hit);
        bool hitAny = traceRayBVH(currRay, tris, nodes, numOfNodes, textures, hit);

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
        fungt::Vec3 newDir = sampleHemisphere(N, fgtRng);
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

#endif // _CORE_RENDERER_H_
