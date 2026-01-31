#include <gtest/gtest.h>
#include "../../Vector/gpu_vec3.hpp"

// Test fixture for gpuVec3
class GpuVec3Test : public ::testing::Test {
protected:
    fungt::gpuVec3 vec;
    
    void SetUp() override {
        vec.clear();
    }
};

TEST_F(GpuVec3Test, InitiallyEmpty) {
    EXPECT_EQ(vec.size(), 0);
}

TEST_F(GpuVec3Test, PushBackAddsElement) {
    fungt::Vec3 v(1.0f, 2.0f, 3.0f);
    vec.push_back(v);
    
    EXPECT_EQ(vec.size(), 1);
    EXPECT_FLOAT_EQ(vec.x[0], 1.0f);
    EXPECT_FLOAT_EQ(vec.y[0], 2.0f);
    EXPECT_FLOAT_EQ(vec.z[0], 3.0f);
}

TEST_F(GpuVec3Test, GetReturnsCorrectValue) {
    fungt::Vec3 v1(1.0f, 2.0f, 3.0f);
    fungt::Vec3 v2(4.0f, 5.0f, 6.0f);
    
    vec.push_back(v1);
    vec.push_back(v2);
    
    fungt::Vec3 retrieved = vec.get(1);
    EXPECT_FLOAT_EQ(retrieved.x, 4.0f);
    EXPECT_FLOAT_EQ(retrieved.y, 5.0f);
    EXPECT_FLOAT_EQ(retrieved.z, 6.0f);
}

TEST_F(GpuVec3Test, SetModifiesElement) {
    vec.push_back(fungt::Vec3(1, 2, 3));
    
    fungt::Vec3 newVal(10.0f, 20.0f, 30.0f);
    vec.set(0, newVal);
    
    EXPECT_FLOAT_EQ(vec.x[0], 10.0f);
    EXPECT_FLOAT_EQ(vec.y[0], 20.0f);
    EXPECT_FLOAT_EQ(vec.z[0], 30.0f);
}

TEST_F(GpuVec3Test, ReserveAllocatesCapacity) {
    vec.reserve(1000);
    
    EXPECT_GE(vec.x.capacity(), 1000);
    EXPECT_GE(vec.y.capacity(), 1000);
    EXPECT_GE(vec.z.capacity(), 1000);
}

TEST_F(GpuVec3Test, ClearRemovesAllElements) {
    vec.push_back(fungt::Vec3(1, 2, 3));
    vec.push_back(fungt::Vec3(4, 5, 6));
    
    vec.clear();
    
    EXPECT_EQ(vec.size(), 0);
}

TEST_F(GpuVec3Test, FillSetsAllElements) {
    vec.resize(5);
    vec.fill(fungt::Vec3(7.0f, 8.0f, 9.0f));
    
    for (size_t i = 0; i < vec.size(); i++) {
        EXPECT_FLOAT_EQ(vec.x[i], 7.0f);
        EXPECT_FLOAT_EQ(vec.y[i], 8.0f);
        EXPECT_FLOAT_EQ(vec.z[i], 9.0f);
    }
}

TEST_F(GpuVec3Test, MemoryLayoutIsContiguous) {
    // Critical for GPU performance!
    vec.push_back(fungt::Vec3(1, 2, 3));
    vec.push_back(fungt::Vec3(4, 5, 6));
    vec.push_back(fungt::Vec3(7, 8, 9));
    
    // Verify arrays are actually separate and contiguous
    float* xPtr = vec.x.data();
    float* yPtr = vec.y.data();
    float* zPtr = vec.z.data();
    
    EXPECT_EQ(xPtr[0], 1.0f);
    EXPECT_EQ(xPtr[1], 4.0f);
    EXPECT_EQ(xPtr[2], 7.0f);
    
    EXPECT_EQ(yPtr[0], 2.0f);
    EXPECT_EQ(yPtr[1], 5.0f);
    EXPECT_EQ(yPtr[2], 8.0f);
}
