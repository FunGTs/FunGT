#if !defined(_SURFACE_HPP_)
#define _SURFACE_HPP_

#include "Renderable/renderable.hpp"
#include "Shaders/shader.hpp"
#include "MarchingCubes/marching_cubes_cpu.hpp"
#include "Path_Manager/path_manager.hpp"
#include "GraphicsRenderBackend/gpu_buffer.hpp"
#include "Material/material.hpp"
#include "GraphicsRenderBackend/vertex_format.hpp"
#include <memory>
#include <functional>

struct SurfaceVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;

    FUNGT_VERTEX_FORMAT(SurfaceVertex, position, normal, color)
};

class Surface : public Renderable {
private:
    std::unique_ptr<Shader> m_shader;

    glm::mat4 m_modelMatrix = glm::mat4(1.f);
    glm::mat4 m_viewMatrix = glm::mat4(1.f);
    glm::mat4 m_projMatrix = glm::mat4(1.f);

    glm::vec3 m_position = glm::vec3(0.f);
    glm::vec3 m_rotation = glm::vec3(0.f);
    glm::vec3 m_scale = glm::vec3(1.f);

    Material m_material;

protected:
    fungt::MarchingCubesCPU m_mc;
    std::unique_ptr<GPUBuffer> m_buffer;
    unsigned int m_indexCount = 0;
    std::string m_fs;
    std::string m_vs;
    Surface();

public:
    ~Surface();

    void setField(std::function<float(fungt::Vec3)> field);
    void sampleScalarFieldInside(fungt::Vec3 min, fungt::Vec3 max);
    void setResolution(int x, int y, int z);
    void setIsovalue(float iso);

    void load();
    virtual void rebuild();

    void position(float x, float y, float z);
    void rotation(float x, float y, float z);
    void scale(float s);
    void setMaterial(const Material& mat);
    Material& getMaterial();

    void draw() override;
    Shader& getShader() override;
    glm::mat4 getModelMatrix() const override;
    glm::mat4 getViewMatrix() override;
    void setViewMatrix(const glm::mat4& v) override;
    void updateModelMatrix() override;
    glm::mat4 getProjectionMatrix() override;

    static std::shared_ptr<Surface> create();
};

#endif // _SURFACE_HPP_
