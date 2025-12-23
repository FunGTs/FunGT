#if !defined(_INTERSECTION_H_)
#define _INTERSECTION_H_
#include "../Ray/ray.hpp"
#include "../HitData/hit_data.hpp"
#include "../../Triangle/triangle.hpp"
#include "../BVH/aabb.hpp"

class Intersection{

    public:
        static fgt_device inline bool MollerTrumbore(const fungt::Ray& ray, const Triangle& tri, float tMin, float tMax, HitData& rec){
            using namespace fungt;

            const float EPSILON = 1e-8f;

            Vec3 edge1 = tri.v1 - tri.v0;
            Vec3 edge2 = tri.v2 - tri.v0;

            Vec3 h = ray.m_dir.cross(edge2);
            float a = edge1.dot(h);
            if (fabs(a) < EPSILON)
                return false; // Ray parallel to triangle

            float f = 1.0f / a;
            Vec3 s = ray.m_origin - tri.v0;
            float u = f * s.dot(h);
            if (u < 0.0f || u > 1.0f)
                return false;

            Vec3 q = s.cross(edge1);
            float v = f * ray.m_dir.dot(q);
            if (v < 0.0f || u + v > 1.0f)
                return false;

            float t = f * edge2.dot(q);
            if (t < tMin || t > tMax)
                return false;

            //   Hit: fill record
            rec.dis = t;
            rec.point = ray.at(t);
            // Store barycentric coordinates (CRITICAL for UV interpolation!)
            rec.bary = fungt::Vec3(1.0f - u - v, u, v);
            // Geometric normal (for safe ray offset)
            rec.geometricNormal = edge1.cross(edge2).normalize();
            // Shading normal (interpolated per-vertex normals for smooth shading)
            rec.normal = (tri.n0 * rec.bary.x +
                tri.n1 * rec.bary.y +
                tri.n2 * rec.bary.z).normalize();
            // Make sure shading normal faces same hemisphere as geometric normal
            if (rec.normal.dot(rec.geometricNormal) < 0.0f) {
                rec.normal = rec.normal * -1.0f;
            }
            return true;
        }
        static fgt_device bool intersectAABB(
            const fungt::Ray& ray,
            const AABB& box,
            float tMin,
            float tMax)
        {
            // Test each axis (X, Y, Z)
            for (int axis = 0; axis < 3; axis++) {
                float invD = 1.0f / ray.m_dir[axis];  // Precompute inverse direction
                float t0 = (box.m_min[axis] - ray.m_origin[axis]) * invD;
                float t1 = (box.m_max[axis] - ray.m_origin[axis]) * invD;

                // Swap if needed (ray direction is negative)
                if (invD < 0.0f) {
                    float temp = t0;
                    t0 = t1;
                    t1 = temp;
                }

                // Update tMin/tMax range
                tMin = fmaxf(t0, tMin);
                tMax = fminf(t1, tMax);

                // Early exit if no overlap
                if (tMax <= tMin)
                    return false;
            }
            return true;
        }


};





#endif // _INTERSECTION_H_
