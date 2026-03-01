#if !defined(_SIMPLE_GEOMETRY_HPP_)
#define _SIMPLE_GEOMETRY_HPP_

#include "Geometries/primitives.hpp"
#include "Geometries/cube.hpp"
#include "Geometries/sphere.hpp"
#include "Geometries/box.hpp"
#include "Geometries/plane.hpp"
#include "Geometries/torus.hpp"
#include "Renderable/renderable.hpp"
#include "Path_Manager/path_manager.hpp"
#include "Material/material.hpp"
#include "Shaders/shader.hpp"
#include <memory>



enum class Geometry {
    Cube,
    Sphere,
    Box,
    Plane,
    Torus
    // Add more geometry types as needed
};

class SimpleGeometry : public Renderable {
private:
    Material m_material;
    std::shared_ptr<Primitive> m_primitive;
    Shader m_Shader;

    std::string m_vs_path;
    std::string m_fs_path;

    glm::mat4 m_ModelMatrix = glm::mat4(1.f);
    glm::mat4 m_ViewMatrix = glm::mat4(1.f);
    glm::mat4 m_ProjectionMatrix = glm::mat4(1.f);

    glm::vec3 m_position = glm::vec3(0.f);
    glm::vec3 m_rotation = glm::vec3(0.f);
    glm::vec3 m_scale = glm::vec3(1.f);
    bool m_isTexturized = false;
    std::string m_animationID;
    SimpleGeometry();

public:
    ~SimpleGeometry();

    // Load methods
    void load(const std::string &pathToTexture = "");
    void setShaderPaths(const std::string &vs_path, const std::string &fs_path);
    void position(float x = 0.f, float y = 0.f, float z = 0.f);
    void rotation(float x = 0.f, float y = 0.f, float z = 0.f);
    void scale(float s = 1.f);
    void scale(float x, float y, float z){
        m_scale = glm::vec3(x, y, z);
        m_ModelMatrix = glm::scale(m_ModelMatrix, m_scale);
    }
    glm::vec3 getScale() const { return m_scale; } // Assuming uniform scaling
    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getRotation() const { return m_rotation; }
    bool isTexturized()const {return m_isTexturized;}
    // Set the primitive (Cube, Sphere, etc.)
    void setPrimitive(std::shared_ptr<Primitive> primitive);
    // Material setters (for custom materials)
    void setMaterial(const glm::vec3& baseColor, float roughness, float metallic);

    // Getters for PBR path tracer
    std::shared_ptr<Primitive> getPrimitive() const { return m_primitive; }
    const Material& getMaterial() const { return m_material; }
    // Override methods from Renderable
    void draw() override;
    Shader& getShader() override;
    glm::mat4 getViewMatrix() override;
    void setViewMatrix(const glm::mat4 &viewMatrix) override;
    void updateModelMatrix() override;
    glm::mat4 getProjectionMatrix() override;
    glm::mat4 getModelMatrix() const override;
    void setAnimationID(const std::string& id) { m_animationID = id; }
    std::string getAnimationID() const { return m_animationID; }
    // Static factory method (like SimpleModel)
    static std::shared_ptr<SimpleGeometry> create(Geometry geomType);
};

#endif // _SIMPLE_GEOMETRY_HPP_
