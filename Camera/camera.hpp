#if !defined(_CAMERA_H_)
#define _CAMERA_H_
#include "include/glmath.hpp"    
#include "Shaders/shader.hpp"

enum keyboardDir {
    FORWRD = 0,
    BACK = 1,
    LEFT = 2,
    RIGHT = 3
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
    //vectors
    glm::vec3 m_vPos;
    glm::vec3 m_vFront;
    glm::vec3 m_vWorldUp;
    glm::vec3 m_vRight;
    glm::vec3 m_vUp;
    glm::vec3 m_vDirection;
    glm::vec3 m_vTarget; // orbit center point

    //rotations
    GLfloat m_pitch = PITCH;
    GLfloat m_yaw = YAW;
    GLfloat m_roll;

    GLfloat m_speed;
    GLfloat m_sens;

    // Blender-style orbit parameters
    float m_distance;     // distance from target
    float m_orbitSpeed;
    float m_panSpeed;
    float m_fov = 45;
private:
    glm::mat4 m_ViewMatrix;

public:
    Camera();
    Camera(glm::vec3 position, glm::vec3 dir, glm::vec3 worldUp);
    ~Camera();

    // Getters
    glm::mat4 getViewMatrix();
    glm::vec3 getPosition();
    glm::vec3 getFront();
    glm::vec3 getUp();
    float getFOV();
    // Blender-style controls
    void orbit(float deltaX, float deltaY);     // MMB drag
    void pan(float deltaX, float deltaY);       // Shift+MMB drag
    void zoom(float delta);                     // Mouse wheel

    // Old FPS-style controls (keep for compatibility)
    void updateInputs(const float& dt, const int dir, const double& offX, const double& offY);
    void updateMouseInput(float& offX, float& offY);
    void move(const float dt, const int dir);
    void updateVectors();
};

#endif // _CAMERA_H_