#if !defined(_SIMPLE_MODEL_HPP_)
#define _SIMPLE_MODEL_HPP_
#include "../Model/model.hpp"
#include "../Triangle/triangle.hpp"
#include "../Renderable/renderable.hpp"
#include "../DataPaths/datapaths.hpp"
#include "../Physics/RigidBody/rigid_body.hpp"
#include "../Physics/CollisionManager/collision_manager.hpp"
#include "../gpu/data/device_pod.hpp"
#include <optional>
class SimpleModel : public Renderable {


    std::shared_ptr<Model> m_model; // Pointer to the Model object
    std::weak_ptr<RigidBody> m_physicsBody; 
    std::string m_path_fs;
    std::string m_path_vs;
    glm::mat4 m_ModelMatrix;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjectionMatrix;
    glm::vec3 m_position = glm::vec3(0.f);
    glm::vec3 m_rotation = glm::vec3(0.f);
    glm::vec3 m_scale    = glm::vec3(1.0); 
    std::vector<Triangle> m_triangles;
    std::string m_animationID;
    bool isStatic = true; // Flag to indicate if the model is static (non-moving)
    bool m_usePhysicsRigidBody = true; // Flag to indicate if the model should use a physics rigid body
    SimpleModel();
public:
  
    ~SimpleModel();
    // Method to set the model
    void load(const ModelPaths &data);
    void LoadModelData(const ModelPaths &data);
    void LoadModel(const ModelPaths &data);
    void InitGraphics();
    void position(float x = 0.f, float y = 0.f, float z = 0.f);
    void rotation(float x = 0.f, float y = 0.f, float z = 0.f);
    void scale(float s = 1.f);
    void scale(float x, float y, float z){
        m_scale = glm::vec3(x, y, z);
        m_ModelMatrix = glm::scale(m_ModelMatrix, m_scale);
    }
    void setUsePhysicsRigidBody(bool use) { m_usePhysicsRigidBody = use; }
    glm::vec3 getScale() const { return m_scale; } // Assuming uniform scaling
    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getRotation() const { return m_rotation; }

    void addCollisionProperty(std::shared_ptr<RigidBody> body);;
    //Override methods from Renderable
    void draw() override;
    Shader& getShader() override;
    glm::mat4 getViewMatrix() override;
    void setViewMatrix(const glm::mat4 &viewMatrix) override;
    void updateModelMatrix() override;
    glm::mat4 getProjectionMatrix() override;
    glm::mat4 getModelMatrix() const override;
    std::vector<Triangle> getTriangleList();
    void setAnimationID(const std::string& id) { m_animationID = id; }
    std::string getAnimationID() const { return m_animationID; }
    const std::vector<std::unique_ptr<Mesh>>& getMeshes() const {
        return m_model->getMeshes();
    }
    Model&  getModel () const {
        return *m_model;
    }
    // Implement the virtual method for physics support
      // Physics support - returns empty weak_ptr by default
    std::weak_ptr<RigidBody> getRigidBody() const override{
        return m_physicsBody;
    }
    static std::shared_ptr<SimpleModel> create() {
        // This works because create() is a MEMBER of SimpleModel
        // and can access private members
        return std::shared_ptr<SimpleModel>(new SimpleModel());
        
        // This doesn't work because make_shared is NOT a member
        // of SimpleModel and tries to call the constructor from outside
        //  return std::make_shared<SimpleModel>(); // Error!
        //std::make_shared is a template function that tries to construct the object internally. 
    }
    

};




#endif // _SIMPLE_MODEL_HPP_ 
