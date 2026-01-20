#if !defined(_DEVICE_DATA_H_)
#define _DEVICE_DATA_H_

namespace gpu
{

    struct DeviceData {
            // Shape data
        int*   shapeType;      // 0 = sphere, 1 = box
        float* radius;       // for spheres
        float* halfExtentX;  // for boxes
        float* halfExtentY;
        float* halfExtentZ;
        float* x_pos;
        float* y_pos;
        float* z_pos;
        float* x_vel;
        float* y_vel;
        float* z_vel;
        float* x_force;
        float* y_force;
        float* z_force;
        float* x_angVel;
        float* y_angVel;
        float* z_angVel;
        float* x_torque;
        float* y_torque;
        float* z_torque;
        float* orientW;
        float* orientX;
        float* orientY;
        float* orientZ;
        float* invMass;
        float* invInertiaTensor;
    };
    
} // namespace gpu


#endif // _DEVICE_DATA_H_
