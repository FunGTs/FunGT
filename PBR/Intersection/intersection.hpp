#if !defined(_INTERSECTION_H_)
#define _INTERSECTION_H_
#include "../Ray/ray.hpp"
#include "../HitData/hit_data.hpp"
#include "../../Triangle/triangle.hpp"

class Intersection{

    public:
        static bool MollerTrumbore(const fungt::Ray& ray, const Triangle& tri, float tMin, float tMax, HitData& rec);

};





#endif // _INTERSECTION_H_
