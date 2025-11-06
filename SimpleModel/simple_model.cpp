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

void SimpleModel::LoadModelData(const ModelPaths& data)
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
    const std::vector<std::unique_ptr<Mesh>>& meshes = m_model->getMeshes();
    size_t totalTriangles = 0;


    for (const auto& meshPtr : meshes) {

        totalTriangles += meshPtr->m_index.size() / 3;
    }
    std::cout << "Total triangles to create: " << totalTriangles << std::endl;
    m_triangles.reserve(totalTriangles);

    for (auto& meshPtr : meshes) {
        std::cout << "Processing : " << meshPtr->m_index.size() << std::endl;
        for (size_t i = 0; i < meshPtr->m_index.size(); i += 3) {
            const funGTVERTEX& v0 = meshPtr->m_vertex[meshPtr->m_index[i + 0]];
            const funGTVERTEX& v1 = meshPtr->m_vertex[meshPtr->m_index[i + 1]];
            const funGTVERTEX& v2 = meshPtr->m_vertex[meshPtr->m_index[i + 2]];
            Triangle tri;
            tri.v0 = fungt::toFungtVec3(v0.position);
            tri.v1 = fungt::toFungtVec3(v1.position);
            tri.v2 = fungt::toFungtVec3(v2.position);

            // Flat face normal (correct for path tracing)
            fungt::Vec3 e1 = tri.v1 - tri.v0;
            fungt::Vec3 e2 = tri.v2 - tri.v0;
            tri.normal = e1.cross(e2).normalize();

            // Include material if present
            if (!meshPtr->m_material.empty()) {

                tri.material.ambient[0] = meshPtr->m_material[0].m_ambientLight.x;
                tri.material.ambient[1] = meshPtr->m_material[0].m_ambientLight.y;
                tri.material.ambient[2] = meshPtr->m_material[0].m_ambientLight.z;

                tri.material.diffuse[0] = meshPtr->m_material[0].m_diffLigth.x;
                tri.material.diffuse[1] = meshPtr->m_material[0].m_diffLigth.y;
                tri.material.diffuse[2] = meshPtr->m_material[0].m_diffLigth.z;

                tri.material.specular[0] = meshPtr->m_material[0].m_specLight.x;
                tri.material.specular[1] = meshPtr->m_material[0].m_specLight.y;
                tri.material.specular[2] = meshPtr->m_material[0].m_specLight.z;

                tri.material.shininess = meshPtr->m_material[0].m_shininess;
                
            }
            else{
                tri.material.ambient[0] = tri.material.ambient[1] = tri.material.ambient[2] = 0.1f;
                tri.material.diffuse[0] = tri.material.diffuse[1] = tri.material.diffuse[2] = 0.7f;
                tri.material.specular[0] = tri.material.specular[1] = tri.material.specular[2] = 0.2f;
                tri.material.shininess = 16.0f;
            }

            // Optional: handle texture-only meshes later when you add albedo maps
            // if (!m_textures.empty()) tri.albedoMap = m_textures[0].id;

            m_triangles.push_back(std::move(tri));
        }
    }

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
