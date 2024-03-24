#include "camera.hpp"

Camera::Camera(){
    std::cout<<"Camera constructor"<<std::endl; 
    //
    m_sens = SENSITIVITY; 
    //View Matrix

    m_ViewMatrix = glm::mat4(1.0);

    //World Up
    m_vUp = glm::vec3(0.0f, 1.0f, 0.0f); 
    m_vWorldUp = m_vUp; 


    m_vPos = glm::vec3(0.f,0.f,3.f); //Location of the camera 
    m_vFront = glm::vec3(0.f,0.0f,-1.f); //where the camera is looking
  

    // //updateVectors(); 

    // //Camera Direction:
    // m_vTarget = glm::vec3(0.f); 
    // m_vDirection = glm::normalize(m_vPos-m_vTarget);

    // //Right axis
    // glm::vec3 up; 
    // up = glm::vec3(0.0,1.0,0.0); //create a vector that points upWard in the world space
    // m_vRight = glm::normalize(glm::cross(up,m_vDirection)); 


    // //Up Axis:

    // m_vUp = glm::cross(m_vDirection,m_vRight); 








    updateVectors(); 
}


Camera::Camera(glm::vec3 position, glm::vec3 dir, glm::vec3 worldUp)
{
    // m_ViewMatrix = glm::mat4(1.f);

    // m_speed = 3.f; 
    // m_sens = 5.f; 

    // m_vWorldUp = worldUp;
    // m_vPos = position; //Camera position 
    // m_vRight = glm::vec3(0.f);
    // m_vFront = worldUp; 

    // m_pitch = 0.f; 
    // m_yaw = 0.f; 
    // m_roll = 0.f; 

    // updateVectors();



}
Camera::~Camera(){
    
    std::cout<<"Camera Destructor"<<std::endl; 
}


//Methods
void Camera::updateVectors() {
   
    //glm::quat quaternion = glm::quat(glm::vec3(glm::radians(m_pitch), glm::radians(m_yaw), 0.0f));
    // Initial front vector pointing towards -Z axis
    //glm::vec3 front(0.0f, 0.0f, -1.0f);
    // Rotate the initial front vector by the quaternion
    //glm::vec3 rotatedFront = quaternion * front;
     glm::vec3 front;
     front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
     front.y = sin(glm::radians(m_pitch));
     front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_vFront = glm::normalize(front);
    //std::cout<<" m_vFront.x : "<<m_vFront.x << " m_vFront.y : " << m_vFront.y <<std::endl; 
    m_vRight = glm::normalize(glm::cross(m_vFront,m_vWorldUp)); 
    m_vUp = glm::cross(m_vRight,m_vFront);

    //Camera Direction:
    //m_vTarget = glm::vec3(0.f); 
   // m_vDirection = glm::normalize(m_vPos-m_vTarget);

    //Right axis
    //glm::vec3 up; 
    //up = glm::vec3(0.0,1.0,0.0); //create a vector that points upWard in the world space
    //m_vRight = glm::normalize(glm::cross(up,m_vDirection)); 


    //Up Axis:

    //m_vUp = glm::cross(m_vDirection,m_vRight); 

}

glm::mat4 Camera::getViewMatrix(){

     //updateVectors();

   
    m_ViewMatrix = glm::lookAt(m_vPos,m_vPos+m_vFront, m_vUp);
  

    return m_ViewMatrix; 
}
glm::vec3 Camera::getPosition(){

    return m_vPos; 

}
 void Camera::move(const float dt, const int dir){

    m_speed = 2.5*dt; 
    switch (dir)
    {
    case FORWRD:
            m_vPos += m_vFront*m_speed;
        break;
    case BACK:
             m_vPos -= m_vFront*m_speed; 
        break;
    case LEFT: 
             m_vPos += glm::normalize(glm::cross(m_vFront,m_vUp))*m_speed; 
        break; 
    case RIGHT:
             m_vPos -= glm::normalize(glm::cross(m_vFront, m_vUp))*m_speed; 
        break;
    
    default:
        break;
    }
 }
 void Camera::updateMouseInput( float &offX,  float &offY){

    offX *= m_sens;
    offY *= m_sens; 

    m_yaw += offX; 
    m_pitch += offY; 

    if(m_pitch > 89.f){
        m_pitch = 89.f;
    }
    if(m_pitch < -89.f){
        m_pitch = -89.f; 
    }

    updateVectors(); 


   
}
void Camera::updateInputs(const float& dt, const int dir, const double &offX, const double &offY ){
     move(dt, dir);
     

}
