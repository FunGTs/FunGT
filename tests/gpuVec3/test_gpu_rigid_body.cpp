#include <gtest/gtest.h>
#include "../../Physics/GPU/gpu_rigid_body.hpp"
#include "../../Physics/GPU/gpu_rigid_body_builder.hpp"

class GPURigidBodyTest : public ::testing::Test {
protected:
    GPURigidBody state;
    
    void SetUp() override {
        state = GPURigidBodyBuilder::createState();
    }
};

TEST_F(GPURigidBodyTest, InitiallyEmpty) {
    EXPECT_EQ(state.numBodies, 0);
}

TEST_F(GPURigidBodyTest, AddSphereIncrementsBodies) {
    int id = GPURigidBodyBuilder::addSphere(
        state,
        fungt::Vec3(0, 0, 0),
        1.0f,  // radius
        1.0f   // mass
    );
    
    EXPECT_EQ(id, 0);
    EXPECT_EQ(state.numBodies, 1);
}

TEST_F(GPURigidBodyTest, SphereHasCorrectProperties) {
    GPURigidBodyBuilder::addSphere(
        state,
        fungt::Vec3(1, 2, 3),
        2.0f,  // radius
        5.0f   // mass
    );
    
    // Check position
    fungt::Vec3 pos = state.positions.get(0);
    EXPECT_FLOAT_EQ(pos.x, 1.0f);
    EXPECT_FLOAT_EQ(pos.y, 2.0f);
    EXPECT_FLOAT_EQ(pos.z, 3.0f);
    
    // Check shape
    EXPECT_EQ(state.shapeTypes[0], 0); // 0 = sphere
    EXPECT_FLOAT_EQ(state.shapeRadii[0], 2.0f);
    
    // Check mass
    EXPECT_FLOAT_EQ(state.invMasses[0], 1.0f / 5.0f);
}

TEST_F(GPURigidBodyTest, StaticBodyHasZeroInvMass) {
    GPURigidBodyBuilder::addBox(
        state,
        fungt::Vec3(0, 0, 0),
        fungt::Vec3(10, 1, 10),
        0.0f  // mass = 0 (static)
    );
    
    EXPECT_FLOAT_EQ(state.invMasses[0], 0.0f);
}

TEST_F(GPURigidBodyTest, AddBoxIncrementsBodies) {
    int id = GPURigidBodyBuilder::addBox(
        state,
        fungt::Vec3(0, 0, 0),
        fungt::Vec3(1, 1, 1),
        1.0f
    );
    
    EXPECT_EQ(id, 0);
    EXPECT_EQ(state.numBodies, 1);
}

TEST_F(GPURigidBodyTest, BoxHasCorrectProperties) {
    GPURigidBodyBuilder::addBox(
        state,
        fungt::Vec3(5, 6, 7),
        fungt::Vec3(2, 3, 4),  // size
        10.0f                  // mass
    );
    
    // Check position
    fungt::Vec3 pos = state.positions.get(0);
    EXPECT_FLOAT_EQ(pos.x, 5.0f);
    EXPECT_FLOAT_EQ(pos.y, 6.0f);
    EXPECT_FLOAT_EQ(pos.z, 7.0f);
    
    // Check shape
    EXPECT_EQ(state.shapeTypes[0], 1); // 1 = box
    fungt::Vec3 size = state.shapeSizes.get(0);
    EXPECT_FLOAT_EQ(size.x, 2.0f);
    EXPECT_FLOAT_EQ(size.y, 3.0f);
    EXPECT_FLOAT_EQ(size.z, 4.0f);
    
    // Check mass
    EXPECT_FLOAT_EQ(state.invMasses[0], 1.0f / 10.0f);
}

TEST_F(GPURigidBodyTest, MultipleObjectsHaveCorrectIndices) {
    int sphere1 = GPURigidBodyBuilder::addSphere(state, fungt::Vec3(0,0,0), 1.0f, 1.0f);
    int box1 = GPURigidBodyBuilder::addBox(state, fungt::Vec3(1,1,1), fungt::Vec3(1,1,1), 1.0f);
    int sphere2 = GPURigidBodyBuilder::addSphere(state, fungt::Vec3(2,2,2), 1.0f, 1.0f);
    
    EXPECT_EQ(sphere1, 0);
    EXPECT_EQ(box1, 1);
    EXPECT_EQ(sphere2, 2);
    EXPECT_EQ(state.numBodies, 3);
}

TEST_F(GPURigidBodyTest, SetVelocityWorks) {
    int id = GPURigidBodyBuilder::addSphere(state, fungt::Vec3(0,0,0), 1.0f, 1.0f);
    
    GPURigidBodyBuilder::setVelocity(state, id, fungt::Vec3(10, 20, 30));
    
    fungt::Vec3 vel = state.velocities.get(id);
    EXPECT_FLOAT_EQ(vel.x, 10.0f);
    EXPECT_FLOAT_EQ(vel.y, 20.0f);
    EXPECT_FLOAT_EQ(vel.z, 30.0f);
}

TEST_F(GPURigidBodyTest, ReserveDoesNotChangeNumBodies) {
    state.reserve(1000);
    
    EXPECT_EQ(state.numBodies, 0);
    EXPECT_GE(state.positions.x.capacity(), 1000);
}

TEST_F(GPURigidBodyTest, AllArraysHaveSameSize) {
    GPURigidBodyBuilder::addSphere(state, fungt::Vec3(0,0,0), 1.0f, 1.0f);
    GPURigidBodyBuilder::addBox(state, fungt::Vec3(1,1,1), fungt::Vec3(1,1,1), 1.0f);
    GPURigidBodyBuilder::addSphere(state, fungt::Vec3(2,2,2), 1.0f, 1.0f);
    
    size_t n = state.numBodies;
    
    EXPECT_EQ(state.positions.size(), n);
    EXPECT_EQ(state.velocities.size(), n);
    EXPECT_EQ(state.forces.size(), n);
    EXPECT_EQ(state.orientations.size(), n);
    EXPECT_EQ(state.invMasses.size(), n);
    EXPECT_EQ(state.shapeTypes.size(), n);
}
