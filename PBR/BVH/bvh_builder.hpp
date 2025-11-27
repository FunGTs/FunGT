#if !defined(_BVH_BUILDER_HPP_)
#define _BVH_BUILDER_HPP_
#include <vector>
#include <iostream>
#include <algorithm>
#include "Triangle/triangle.hpp"
#include "PBR/BVH/aabb.hpp"
#include "PBR/BVH/bvh_node.hpp"
struct TriangleBounds {
    AABB bounds;
    fungt::Vec3 centroid;
    int originalIndex;  // Index in your original triangle array
};
class BVHBuilder{
    const int   NUM_BUCKETS = 12;
    const float TRAVERSAL_COST = 1.0f;
    const float INTERSECTION_COST = 1.0f;
    std::vector<BVHNode> m_nodes;
    std::vector<int> m_triIndices;  // Reordered triangle indices
    public:
        BVHBuilder();
        ~BVHBuilder();


        void build(const std::vector<Triangle>& triangles);

    private:
        TriangleBounds computeTriangleBounds(const Triangle triangle, int index);
        int buildRecursive(std::vector<TriangleBounds>& triBounds, int start, int end);
        int partition(std::vector<TriangleBounds>& triBounds, int start, int end, int axis);
    public:    
        std::vector<BVHNode> moveNodes();
        std::vector<int> moveIndices();

};



#endif // _BVH_BUILDER_HPP_
