#if !defined(_GPU_HOST_DATA_)
#define _GPU_HOST_DATA_


#include <vector>

namespace gpu {

    struct HostData {
        // Shape data
        std::vector<int>   shapeType;
        std::vector<int>   bodyMode;
        std::vector<float> radius;
        std::vector<float> halfExtentX;
        std::vector<float> halfExtentY;
        std::vector<float> halfExtentZ;

        // Positions
        std::vector<float> x_pos;
        std::vector<float> y_pos;
        std::vector<float> z_pos;

        // Linear velocity
        std::vector<float> x_vel;
        std::vector<float> y_vel;
        std::vector<float> z_vel;

        // Forces
        std::vector<float> x_force;
        std::vector<float> y_force;
        std::vector<float> z_force;

        // Angular velocity
        std::vector<float> x_angVel;
        std::vector<float> y_angVel;
        std::vector<float> z_angVel;

        // Torques
        std::vector<float> x_torque;
        std::vector<float> y_torque;
        std::vector<float> z_torque;

        // Orientations (quaternion)
        std::vector<float> orientW;
        std::vector<float> orientX;
        std::vector<float> orientY;
        std::vector<float> orientZ;

        // Mass properties
        std::vector<float> invMass;
        std::vector<float> invInertiaTensor;  // 9 floats per body

        // Material
        std::vector<float> restitution;
        std::vector<float> friction;

        void resize(int capacity) {
            shapeType.resize(capacity, 0);
            bodyMode.resize(capacity, 0);
            radius.resize(capacity, 0.0f);
            halfExtentX.resize(capacity, 0.0f);
            halfExtentY.resize(capacity, 0.0f);
            halfExtentZ.resize(capacity, 0.0f);

            x_pos.resize(capacity, 0.0f);
            y_pos.resize(capacity, 0.0f);
            z_pos.resize(capacity, 0.0f);

            x_vel.resize(capacity, 0.0f);
            y_vel.resize(capacity, 0.0f);
            z_vel.resize(capacity, 0.0f);

            x_force.resize(capacity, 0.0f);
            y_force.resize(capacity, 0.0f);
            z_force.resize(capacity, 0.0f);

            x_angVel.resize(capacity, 0.0f);
            y_angVel.resize(capacity, 0.0f);
            z_angVel.resize(capacity, 0.0f);

            x_torque.resize(capacity, 0.0f);
            y_torque.resize(capacity, 0.0f);
            z_torque.resize(capacity, 0.0f);

            orientW.resize(capacity, 1.0f);  // identity quaternion w=1
            orientX.resize(capacity, 0.0f);
            orientY.resize(capacity, 0.0f);
            orientZ.resize(capacity, 0.0f);

            invMass.resize(capacity, 0.0f);
            invInertiaTensor.resize(capacity * 9, 0.0f);

            restitution.resize(capacity, 0.0f);
            friction.resize(capacity, 0.0f);
        }
    };

} // namespace gpu



#endif // _GPU_HOST_DATA_
