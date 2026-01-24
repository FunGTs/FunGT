#if !defined(_SIMPLE_GEOMETRY_HPP_)
#define _SIMPLE_GEOMETRY_HPP_

#include "Geometries/primitives.hpp"
#include "Geometries/cube.hpp"
#include "Geometries/sphere.hpp"
#include "Geometries/box.hpp"
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

    SimpleGeometry();

public:
    ~SimpleGeometry();

    // Load methods
    void load(const std::string &pathToTexture);
    void setShaderPaths(const std::string &vs_path, const std::string &fs_path);
    void position(float x = 0.f, float y = 0.f, float z = 0.f);
    void rotation(float x = 0.f, float y = 0.f, float z = 0.f);
    void scale(float s = 1.f);

    // Set the primitive (Cube, Sphere, etc.)
    void setPrimitive(std::shared_ptr<Primitive> primitive);

    // Override methods from Renderable
    void draw() override;
    Shader& getShader() override;
    glm::mat4 getViewMatrix() override;
    void setViewMatrix(const glm::mat4 &viewMatrix) override;
    void updateModelMatrix() override;
    glm::mat4 getProjectionMatrix() override;
    glm::mat4 getModelMatrix() override;

    // Static factory method (like SimpleModel)
    static std::shared_ptr<SimpleGeometry> create(Geometry geomType);
};

#endif // _SIMPLE_GEOMETRY_HPP_
