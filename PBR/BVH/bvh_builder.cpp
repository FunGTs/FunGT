#include "bvh_builder.hpp"

BVHBuilder::BVHBuilder()
{
}

BVHBuilder::~BVHBuilder()
{
}

void BVHBuilder::build(const std::vector<Triangle>& triangles) {

    if(triangles.empty()){
        return;
    }

    //Compute the bounds for each triangle
    std::vector<TriangleBounds> tri_bounds(triangles.size());

    for(int i = 0; i<triangles.size(); i++){
        tri_bounds[i] = computeTriangleBounds(triangles[i],i);
    }

    // Reserve space (max nodes = 2 * numTriangles - 1)
    m_nodes.reserve(triangles.size() * 2);
    m_triIndices.reserve(triangles.size());

    // Step 2: Recursively build tree
    buildRecursive(tri_bounds, 0, triangles.size());
    // VALIDATION - ADD THIS
    // std::cout << "========== BVH Validation ==========" << std::endl;
    // std::cout << "Nodes: " << m_nodes.size() << std::endl;
    // std::cout << "Triangle indices: " << m_triIndices.size() << std::endl;
    // std::cout << "Original triangles: " << triangles.size() << std::endl;
    int leafCount = 0;
    int internalCount = 0;
    int totalLeafTris = 0;
    int maxTriIdx = -1;
    int minTriIdx = INT_MAX;

    for (size_t i = 0; i < m_nodes.size(); i++) {
        const BVHNode& node = m_nodes[i];

        if (node.isLeaf()) {
            leafCount++;
            totalLeafTris += node.triCount;

            // std::cout << "Leaf " << i << ": firstTriIdx=" << node.firstTriIdx
            //     << ", triCount=" << node.triCount << std::endl;

            // Check bounds
            if (node.firstTriIdx < 0) {
                //std::cerr << "ERROR: Negative firstTriIdx in leaf " << i << std::endl;
            }
            if (node.firstTriIdx >= (int)m_triIndices.size()) {
                //std::cerr << "ERROR: firstTriIdx out of bounds in leaf " << i << std::endl;
            }
            if (node.firstTriIdx + node.triCount > (int)m_triIndices.size()) {
                //std::cerr << "ERROR: Leaf " << i << " extends past triIndices array" << std::endl;
            }

            // Check triangle indices themselves
            for (int j = 0; j < node.triCount; j++) {
                int idx = m_triIndices[node.firstTriIdx + j];
                maxTriIdx = std::max(maxTriIdx, idx);
                minTriIdx = std::min(minTriIdx, idx);

                if (idx < 0 || idx >= (int)triangles.size()) {
                    //std::cerr << "ERROR: Invalid triangle index " << idx
                      //  << " in leaf " << i << std::endl;
                }
            }
        }
        else {
            internalCount++;

            // std::cout << "Internal " << i << ": left=" << node.leftChild
            //     << ", right=" << node.rightChild << std::endl;

            // Check child indices
            if (node.leftChild < 0 || node.leftChild >= (int)m_nodes.size()) {
                //std::cerr << "ERROR: Invalid leftChild in node " << i << std::endl;
            }
            if (node.rightChild < 0 || node.rightChild >= (int)m_nodes.size()) {
                //std::cerr << "ERROR: Invalid rightChild in node " << i << std::endl;
            }
        }
    }

    // std::cout << "Leaf nodes: " << leafCount << std::endl;
    // std::cout << "Internal nodes: " << internalCount << std::endl;
    // std::cout << "Total leaf triangles: " << totalLeafTris << std::endl;
    // std::cout << "Triangle index range: [" << minTriIdx << ", " << maxTriIdx << "]" << std::endl;
    // std::cout << "=====================================" << std::endl;

    if (totalLeafTris != (int)triangles.size()) {
        std::cerr << "FATAL: Triangle count mismatch!" << std::endl;
    }
}

TriangleBounds  BVHBuilder::computeTriangleBounds(const Triangle triangle, int index) {
    TriangleBounds boundary; 


    boundary.originalIndex = index;
    boundary.bounds.grow(triangle.v0); 
    boundary.bounds.grow(triangle.v1);
    boundary.bounds.grow(triangle.v2);

    // Centroid = average of vertices
    boundary.centroid = (triangle.v0 + triangle.v1 + triangle.v2) * (1.0f / 3.0f);

    return boundary;
}

int BVHBuilder::buildRecursive(std::vector<TriangleBounds>& triBounds, int start, int end){

    // SAFETY CHECK
    if (start >= end) {
        std::cerr << "ERROR: Empty range [" << start << ", " << end << ")" << std::endl;
        return -1;
    }

    int nodeIdx = m_nodes.size();
    m_nodes.emplace_back();

    // Step 1: Compute bounding box for this range
    AABB centroidBounds;
    for (int i = start; i < end; i++) {
        m_nodes[nodeIdx].m_boundingBox.grow(triBounds[i].bounds);
        centroidBounds.grow(triBounds[i].centroid);
    }

    int triCount = end - start;
    const int MAX_LEAF_SIZE = 4;

    if (triCount <= MAX_LEAF_SIZE) {
        // LEAF NODE
        m_nodes[nodeIdx].firstTriIdx = m_triIndices.size();
        m_nodes[nodeIdx].triCount = triCount;

        for (int i = start; i < end; i++) {
            m_triIndices.push_back(triBounds[i].originalIndex);
        }

        return nodeIdx;
    }

    fungt::Vec3 extent = centroidBounds.m_max - centroidBounds.m_min;
    int axis = 0;
    if (extent.y > extent.x) axis = 1;
    if (extent.z > extent[axis]) axis = 2;

    if (centroidBounds.m_max[axis] == centroidBounds.m_min[axis]) {
        // DEGENERATE CASE: make it a leaf
        m_nodes[nodeIdx].firstTriIdx = m_triIndices.size();
        m_nodes[nodeIdx].triCount = triCount;

        for (int i = start; i < end; i++) {
            m_triIndices.push_back(triBounds[i].originalIndex);
        }

        return nodeIdx;
    }

    int mid = partition(triBounds, start, end, axis);

    if (mid == start || mid == end) {
        std::cerr << "WARNING: Bad partition mid=" << mid
            << " for range [" << start << ", " << end << ")" << std::endl;
        mid = (start + end) / 2;
    }

    // Build children
    int leftIdx = buildRecursive(triBounds, start, mid);
    int rightIdx = buildRecursive(triBounds, mid, end);

    // Set children (DON'T set triCount - it's already 0 from initialization)
    m_nodes[nodeIdx].leftChild = leftIdx;
    m_nodes[nodeIdx].rightChild = rightIdx;
    // triCount remains 0 (which makes isLeaf() return false)

    return nodeIdx;
}
// Partition using SAH (Surface Area Heuristic) - builds quality BVH
int BVHBuilder::partition(std::vector<TriangleBounds>& triBounds, int start, int end, int axis)
{
    // // Compute centroid bounds
    // AABB centroidBounds;
    // for (int i = start; i < end; i++) {
    //     centroidBounds.grow(triBounds[i].centroid);
    // }
    // // std::cout << "Partition called: start=" << start << ", end=" << end
    // //     << ", axis=" << axis << std::endl;
    // // std::cout << "  Centroid range: [" << centroidBounds.m_min[axis]
    // //     << ", " << centroidBounds.m_max[axis] << "]" << std::endl;
    // // Initialize buckets
    // struct Bucket {
    //     int count = 0;
    //     AABB bounds;
    // };
    // Bucket buckets[NUM_BUCKETS];

    // // Put triangles into buckets
    // for (int i = start; i < end; i++) {
    //     float centroid = triBounds[i].centroid[axis];
    //     int bucketIdx = NUM_BUCKETS *
    //         ((centroid - centroidBounds.m_min[axis]) /
    //             (centroidBounds.m_max[axis] - centroidBounds.m_min[axis]));

    //     if (bucketIdx == NUM_BUCKETS) bucketIdx = NUM_BUCKETS - 1;

    //     buckets[bucketIdx].count++;
    //     buckets[bucketIdx].bounds.grow(triBounds[i].bounds);
    // }

    // // Compute costs for each split position
    // float costs[NUM_BUCKETS - 1];
    // for (int i = 0; i < NUM_BUCKETS - 1; i++) {
    //     AABB b0, b1;
    //     int count0 = 0, count1 = 0;

    //     // Left side
    //     for (int j = 0; j <= i; j++) {
    //         b0.grow(buckets[j].bounds);
    //         count0 += buckets[j].count;
    //     }

    //     // Right side
    //     for (int j = i + 1; j < NUM_BUCKETS; j++) {
    //         b1.grow(buckets[j].bounds);
    //         count1 += buckets[j].count;
    //     }

    //     // SAH cost = traversal + (left_prob * left_count + right_prob * right_count)
    //     costs[i] = TRAVERSAL_COST +
    //         INTERSECTION_COST * (count0 * b0.surfaceArea() +
    //             count1 * b1.surfaceArea());
    // }

    // // Find bucket with minimum cost
    // float minCost = costs[0];
    // int minCostIdx = 0;
    // for (int i = 1; i < NUM_BUCKETS - 1; i++) {
    //     if (costs[i] < minCost) {
    //         minCost = costs[i];
    //         minCostIdx = i;
    //     }
    // }

    // // Partition triangles based on best split
    // int mid = start;
    // for (int i = start; i < end; i++) {
    //     float centroid = triBounds[i].centroid[axis];
    //     int bucketIdx = NUM_BUCKETS *
    //         ((centroid - centroidBounds.m_min[axis]) /
    //             (centroidBounds.m_max[axis] - centroidBounds.m_min[axis]));

    //     if (bucketIdx == NUM_BUCKETS) bucketIdx = NUM_BUCKETS - 1;

    //     if (bucketIdx <= minCostIdx) {
    //         std::swap(triBounds[i], triBounds[mid]);
    //         mid++;
    //     }
    // }

    // // Fallback to middle if partition failed
    // if (mid == start || mid == end) {
    //     mid = (start + end) / 2;
    // }
    // //std::cout << "  Partition result: mid=" << mid << std::endl;
    // return mid;



    int mid = (start + end) / 2;

    std::nth_element(
        triBounds.begin() + start,
        triBounds.begin() + mid,
        triBounds.begin() + end,
        [axis](const TriangleBounds& a, const TriangleBounds& b) {
            return a.centroid[axis] < b.centroid[axis];
        }
    );

    return mid;
}

std::vector<BVHNode> BVHBuilder::moveNodes()
{
    return std::move(m_nodes);
}

std::vector<int> BVHBuilder::moveIndices()
{
    return std::move(m_triIndices);
}
