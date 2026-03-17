#if !defined(_GPU_GEOMETRY_HPP_)
#define _GPU_GEOMETRY_HPP_

#include "Renderable/renderable.hpp"
#include "Geometries/primitives.hpp"
#include "Geometries/cube.hpp"
#include "Geometries/sphere.hpp"
#include "Geometries/box.hpp"
#include "Shaders/shader.hpp"
#include "Path_Manager/path_manager.hpp"
#include "gpu_collision_manger.hpp"
#include <memory>

enum class GPUGeometryType {
    Cube,
    Sphere,
    Box,
    Plane
};

class GPUGeometry : public Renderable {
private:
    std::shared_ptr<Primitive> m_primitive;
    Shader m_shader;
    std::string m_vs_path;
    std::string m_fs_path;

    std::shared_ptr<gpu::CollisionManager> m_collision;
    int m_startIndex;
    int m_instanceCount;

    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjectionMatrix;

    GPUGeometry(std::shared_ptr<gpu::CollisionManager> collision, int startIndex, int count);

public:
    ~GPUGeometry();

    // Setup methods (like SimpleGeometry!)
    void setPrimitive(std::shared_ptr<Primitive> primitive);
    void setShaderPaths(const std::string& vs_path, const std::string& fs_path);
    void load(const std::string& pathToTexture);

    // Renderable interface
    void draw() override;
    Shader& getShader() override;
    glm::mat4 getModelMatrix() override;
    void updateModelMatrix() override;
    glm::mat4 getViewMatrix() override;
    void setViewMatrix(const glm::mat4& viewMatrix) override;
    glm::mat4 getProjectionMatrix() override;

    // Accessors
    int getStartIndex() const { return m_startIndex; }
    int getInstanceCount() const { return m_instanceCount; }

    // Static factory method (like SimpleGeometry!)
    static std::shared_ptr<GPUGeometry> create(
        std::shared_ptr<gpu::CollisionManager> collision,
        GPUGeometryType geomType,
        std::shared_ptr<Primitive> _primitive = nullptr
    );
};

#endif