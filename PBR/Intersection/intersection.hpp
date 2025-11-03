#if !defined(_INTERSECTION_H_)
#define _INTERSECTION_H_
#include "../Ray/ray.hpp"
#include "../HitData/hit_data.hpp"
#include "../../Triangle/triangle.hpp"

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
            rec.normal = tri.normal;
            rec.materialPtr = &tri.material;

            return true;
        }

};





#endif // _INTERSECTION_H_
