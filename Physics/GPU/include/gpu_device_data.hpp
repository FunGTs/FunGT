#if !defined(_DEVICE_DATA_H_)
#define _DEVICE_DATA_H_

namespace gpu
{
  
    struct DeviceData {
            // Shape data
        int*   shapeType = nullptr;     // 0 = sphere, 1 = box
        int*   bodyMode = nullptr;      // 0 = STATIC, 1 = DYNAMIC
        float* radius = nullptr;        // for spheres
        float* halfExtentX = nullptr;   // for boxes
        float* halfExtentY = nullptr;
        float* halfExtentZ = nullptr;
        float* x_pos = nullptr;
        float* y_pos = nullptr;
        float* z_pos = nullptr;
        float* x_vel = nullptr;
        float* y_vel = nullptr;
        float* z_vel = nullptr;
        float* x_force = nullptr;
        float* y_force = nullptr;
        float* z_force = nullptr;
        float* x_angVel = nullptr;
        float* y_angVel = nullptr;
        float* z_angVel = nullptr;
        float* x_torque = nullptr;
        float* y_torque = nullptr;
        float* z_torque = nullptr;
        float* orientW = nullptr;
        float* orientX = nullptr;
        float* orientY = nullptr;
        float* orientZ = nullptr;
        float* invMass = nullptr;
        float* invInertiaTensor = nullptr;
        float* restitution = nullptr;       //
        float* friction = nullptr;          //
        // Jacobi solver accumulators
        float* dx_vel = nullptr;
        float* dy_vel = nullptr;
        float* dz_vel = nullptr;
        float* dx_angVel = nullptr;
        float* dy_angVel = nullptr;
        float* dz_angVel = nullptr;
    };
    
} // namespace gpu


#endif // _DEVICE_DATA_H_
