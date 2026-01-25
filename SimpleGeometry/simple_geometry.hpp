#if !defined(_SIMPLE_GEOMETRY_HPP_)
#define _SIMPLE_GEOMETRY_HPP_

#include "Geometries/primitives.hpp"
#include "Geometries/cube.hpp"
#include "Geometries/sphere.hpp"
#include "Geometries/box.hpp"
#include "Geometries/plane.hpp"
#include "Renderable/renderable.hpp"
#include "Path_Manager/path_manager.hpp"
#include "Shaders/shader.hpp"
#include <memory>



enum class Geometry {
    Cube,
    Sphere,
    Box,
    Plane
    // Add more geometry types as needed
};

class SimpleGeometry : public Renderable {
private:
    struct GeometryMaterial {
        int baseColorTexIdx = -1;
        glm::vec3 baseColor = glm::vec3(0.8f);
        float roughness = 0.5f;
        float metallic = 0.0f;
    };
    GeometryMaterial m_material;
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
    SimpleGeometry();

public:
    ~SimpleGeometry();

    // Load methods
    void load(const std::string &pathToTexture = "");
    void setShaderPaths(const std::string &vs_path, const std::string &fs_path);
    void position(float x = 0.f, float y = 0.f, float z = 0.f);
    void rotation(float x = 0.f, float y = 0.f, float z = 0.f);
    void scale(float s = 1.f);
    bool isTexturized()const {return m_isTexturized;}
    // Set the primitive (Cube, Sphere, etc.)
    void setPrimitive(std::shared_ptr<Primitive> primitive);
    // Material setters (for custom materials)
    void setMaterial(const glm::vec3& baseColor, float roughness, float metallic);

    // Getters for PBR path tracer
    std::shared_ptr<Primitive> getPrimitive() const { return m_primitive; }
    const GeometryMaterial& getMaterial() const { return m_material; }
    // Override methods from Renderable
    void draw() override;
    Shader& getShader() override;
    glm::mat4 getViewMatrix() override;
    void setViewMatrix(const glm::mat4 &viewMatrix) override;
    void updateModelMatrix() override;
    glm::mat4 getProjectionMatrix() override;
    glm::mat4 getModelMatrix() const override;

    // Static factory method (like SimpleModel)
    static std::shared_ptr<SimpleGeometry> create(Geometry geomType);
};

#endif // _SIMPLE_GEOMETRY_HPP_
