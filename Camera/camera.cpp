#include "camera.hpp"

Camera::Camera() {
    std::cout << "Camera constructor" << std::endl;

    m_sens = SENSITIVITY;
    m_ViewMatrix = glm::mat4(1.0);
    m_vUp = glm::vec3(0.0f, 1.0f, 0.0f);
    m_vWorldUp = m_vUp;

    // Blender-style setup
    m_vTarget = glm::vec3(0.f, 0.f, 0.f);  // Look at origin
    m_distance = 15.0f;                     // Start 15 units away
    m_yaw = 45.0f;                          // 45 degrees horizontal (positive for correct angle)
    m_pitch = 35.0f;                        // 35 degrees looking down
    m_orbitSpeed = 0.3f;
    m_panSpeed = 0.01f;

    updateVectors();
}

Camera::Camera(glm::vec3 position, glm::vec3 dir, glm::vec3 worldUp)
{
    // Keep for compatibility
}

Camera::~Camera() {
    std::cout << "Camera Destructor" << std::endl;
}

void Camera::updateVectors() {
    // Calculate position based on spherical coordinates around target
    float yawRad = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);

    glm::vec3 offset;
    offset.x = m_distance * cos(pitchRad) * cos(yawRad);
    offset.y = m_distance * sin(pitchRad);
    offset.z = m_distance * cos(pitchRad) * sin(yawRad);

    m_vPos = m_vTarget + offset;
    m_vFront = glm::normalize(m_vTarget - m_vPos);
    m_vRight = glm::normalize(glm::cross(m_vFront, m_vWorldUp));
    m_vUp = glm::cross(m_vRight, m_vFront);
}

void Camera::orbit(float deltaX, float deltaY) {
    m_yaw -= deltaX * m_orbitSpeed;
    m_pitch += deltaY * m_orbitSpeed;

    // Clamp pitch
    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    updateVectors();
}

void Camera::pan(float deltaX, float deltaY) {
    float panFactor = m_distance * m_panSpeed;
    m_vTarget -= m_vRight * deltaX * panFactor;
    m_vTarget += m_vUp * deltaY * panFactor;
    updateVectors();
}

void Camera::zoom(float delta) {
    m_distance -= delta * 0.5f;
    if (m_distance < 1.0f) m_distance = 1.0f;
    if (m_distance > 100.0f) m_distance = 100.0f;
    updateVectors();
}

glm::mat4 Camera::getViewMatrix() {
    m_ViewMatrix = glm::lookAt(m_vPos, m_vTarget, m_vUp);
    return m_ViewMatrix;
}

glm::vec3 Camera::getPosition() {
    return m_vPos;
}

glm::vec3 Camera::getFront()
{
    return m_vFront;
}

glm::vec3 Camera::getUp()
{
    return m_vUp;
}
float Camera::getFOV()
{
    return m_fov;
}
void Camera::move(const float dt, const int dir) {
    m_speed = 2.5 * dt;
    switch (dir)
    {
    case FORWRD:
        m_vPos += m_vFront * m_speed;
        break;
    case BACK:
        m_vPos -= m_vFront * m_speed;
        break;
    case LEFT:
        m_vPos += glm::normalize(glm::cross(m_vFront, m_vUp)) * m_speed;
        break;
    case RIGHT:
        m_vPos -= glm::normalize(glm::cross(m_vFront, m_vUp)) * m_speed;
        break;
    default:
        break;
    }
}

void Camera::updateMouseInput(float& offX, float& offY) {
    offX *= m_sens;
    offY *= m_sens;
    m_yaw += offX;
    m_pitch += offY;

    if (m_pitch > 89.f) m_pitch = 89.f;
    if (m_pitch < -89.f) m_pitch = -89.f;

    updateVectors();
}

void Camera::updateInputs(const float& dt, const int dir, const double& offX, const double& offY) {
    move(dt, dir);
}