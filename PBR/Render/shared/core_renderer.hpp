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
