#if !defined(_CAMERA_H_)
#define _CAMERA_H_
#include <glm/gtc/quaternion.hpp>
#include "../Shaders/shader.hpp"

/*
    Camera Class: 




*/
enum keyboardDir{
    FORWRD = 0, 
    BACK = 1, 
    LEFT = 2, 
    RIGHT = 3
};
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;
class Camera{
    public:
        //vectors
        glm::vec3 m_vPos; 
        glm::vec3 m_vFront; 
        glm::vec3 m_vWorldUp; 
        glm::vec3 m_vRight; 
        glm::vec3 m_vUp;
        glm::vec3 m_vDirection; 
        glm::vec3 m_vTarget; //where is the object our camera is looking at? 
        //rotations
         GLfloat m_pitch = PITCH; //rotation in x, look up or down
         GLfloat m_yaw = YAW; //rotation in y, look right or left
         GLfloat m_roll; //rotation in z, 

                 
        GLfloat m_speed; 
        GLfloat m_sens;



    private: 
        //Matrices
        glm::mat4 m_ViewMatrix; 



        void updateVectors(); 


    public: 
        Camera();
        Camera(glm::vec3 position, glm::vec3 dir, glm::vec3 worldUp );
        ~Camera();

        //Getters; 
        glm::mat4 getViewMatrix();
        glm::vec3 getPosition(); 
        void updateInputs(const float& dt, const int dir, const double &offX, const double &offY);
        void updateMouseInput( float &offX,  float &offY);
        void move(const float dt, const int dir);




}; 

#endif // _CAMERA_H_