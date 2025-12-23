#if !defined(_BVH_NODE_H_)
#define _BVH_NODE_H_
#include "aabb.hpp"
class BVHNode{

public:
    AABB m_boundingBox;

    int leftChild = -1;   // Initialize to -1
    int rightChild = -1;  // Initialize to -1
    int firstTriIdx = -1;
    int triCount = 0;     // Initialize to 0

    fgt_device bool isLeaf() const {
        return triCount > 0;
    }
};

#endif // _BVH_NODE_H_
