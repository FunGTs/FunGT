#include "simple_model.hpp"


SimpleModel::SimpleModel() {
    m_model = std::make_shared<Model>();
    
}
SimpleModel::~SimpleModel() {
    // Destructor logic if needed
}

void SimpleModel::load(const ModelPaths &data) {

    
    m_model->loadModelData(data.path);
    // m_model->loadModel(data.path);
    m_model->createShader(data.vs_path, data.fs_path);
    m_model->InitGraphics();
    
}

void SimpleModel::LoadModel(const ModelPaths& data)
{
   m_model->loadModelData(data.path);
   m_model->setDirPath(data.path);
   std::cout<<"SUCCESS: " << std::endl;
}

void SimpleModel::LoadModel(const ModelPaths &data)
{
    m_model->loadModelData(data.path);
    m_path_vs = data.vs_path;
    m_path_fs = data.fs_path;
    m_model->setDirPath(data.path);
}

void SimpleModel::InitGraphics()
{
    m_model->createShader(m_path_vs, m_path_fs);
    m_model->InitGraphics();
}

void SimpleModel::draw()
{
    m_model->draw();
}

Shader &SimpleModel::getShader()
{
    return m_model->getShader();
}

glm::mat4 SimpleModel::getViewMatrix()
{
    return m_ViewMatrix;
}

void SimpleModel::setViewMatrix(const glm::mat4 &viewMatrix)
{
    m_ViewMatrix = viewMatrix;
}

void SimpleModel::updateModelMatrix()
{
    if(m_collisionM.has_value()){
       
        //std::cout<<"Updating position from Collision Manager"<<std::endl;
        auto rigidBoy = (*m_collisionM)->getCollideBody(0); // Assuming the first body is the one we want
        if(!rigidBoy){
            std::cerr<<"Error: RigidBody is nullptr in SimpleModel::updateModelMatrix"<<std::endl;
            
        }
        
        //std::cout<<"RigidBody Position: ("<<rigidBoy->m_pos.x<<", "<<rigidBoy->m_pos.y<<", "<<rigidBoy->m_pos.z<<")\n";
        m_position.x = rigidBoy->m_pos.x;
        m_position.y = rigidBoy->m_pos.y;
        m_position.z = rigidBoy->m_pos.z;
        auto eulerAngles = rigidBoy->getEulerAngles();
        //std::cout<<"RigidBody Rotation (Euler angles in degrees): ("<<glm::degrees(eulerAngles.x)<<", "
        //<<glm::degrees(eulerAngles.y)<<", "<<glm::degrees(eulerAngles.z)<<")\n";
        // Convert radians to degrees for glm
        m_rotation.x = glm::degrees(eulerAngles.x);
        m_rotation.y = glm::degrees(eulerAngles.y);
        m_rotation.z = glm::degrees(eulerAngles.z);
    }   
    //m_rotation.x = (float)glfwGetTime()*10.0;
    //m_rotation.z = (float)glfwGetTime()*10.0;
    m_ModelMatrix = glm::mat4(1.f);
    m_ModelMatrix = glm::translate(m_ModelMatrix, m_position);
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.x), glm::vec3(1.f, 0.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.y), glm::vec3(0.f, 1.f, 0.f)); 
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.z), glm::vec3(0.f, 0.f, 1.f));
    m_ModelMatrix = glm::scale(m_ModelMatrix, m_scale);
}

glm::mat4 SimpleModel::getProjectionMatrix()
{
    return m_ProjectionMatrix;
}

glm::mat4 SimpleModel::getModelMatrix()
{
    return m_ModelMatrix;
}

std::vector<Triangle> SimpleModel::getTriangleList()
{
    size_t totalTriangles = 0;
    for (const auto& meshPtr : m_model->getMeshes()) {
        totalTriangles += meshPtr->m_index.size() / 3;
    }
    m_triangles.reserve(totalTriangles);


    return m_triangles;
}

void SimpleModel::position(float x, float y, float z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
    m_ModelMatrix = glm::translate(m_ModelMatrix, m_position);
}

void SimpleModel::rotation(float x, float y, float z)
{
    m_rotation.x = x;
    m_rotation.y = y;
    m_rotation.z = z;
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.x), glm::vec3(1.f, 0.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.y), glm::vec3(0.f, 1.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.z), glm::vec3(0.f, 0.f, 1.f));
}

void SimpleModel::scale(float s)
{
    m_scale = glm::vec3(s);
    m_ModelMatrix = glm::scale(m_ModelMatrix, m_scale);
}

void SimpleModel::addCollisionProperty(std::shared_ptr<CollisionManager> _collisionM)
{
    m_collisionM = _collisionM; // Assign the CollisionManager to the optional
}
