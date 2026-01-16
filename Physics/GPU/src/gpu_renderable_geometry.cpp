#include "../include/gpu_renderable_geometry.hpp"

GPUGeometry::GPUGeometry(std::shared_ptr<gpu::CollisionManager> collision,
    int startIndex,
    int count)
    : m_collision(collision), m_startIndex(startIndex), m_instanceCount(count) {
}

GPUGeometry::~GPUGeometry() {
}

void GPUGeometry::setPrimitive(std::shared_ptr<Primitive> primitive) {
    m_primitive = primitive;
}

void GPUGeometry::setShaderPaths(const std::string& vs_path, const std::string& fs_path) {
    m_vs_path = vs_path;
    m_fs_path = fs_path;
}

void GPUGeometry::load(const std::string& pathToTexture) {
    if (!m_primitive) {
        throw std::runtime_error("GPUGeometry::load() - No primitive set!");
    }

    m_primitive->setData();
    m_shader.create(m_vs_path, m_fs_path);
    m_primitive->setTexture(pathToTexture);
    m_primitive->InitGraphics();
}

void GPUGeometry::draw() {
    if (!m_primitive) return;

    m_shader.Bind();

    // Get kernel from collision manager
    auto kernel = m_collision->getKernel();

    // Bind SSBO
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, kernel->getModelMatrixSSBO());

    // Set uniforms
    m_shader.setUniformMat4fv("ViewMatrix", m_ViewMatrix);
    m_shader.setUniformMat4fv("ProjectionMatrix", m_ProjectionMatrix);

    // Draw
    m_primitive->draw();
}

Shader& GPUGeometry::getShader() {
    return m_shader;
}

glm::mat4 GPUGeometry::getModelMatrix() {
    return glm::mat4(1.0f);
}

void GPUGeometry::updateModelMatrix() {
    // GPU handles matrices - nothing to do!
}

glm::mat4 GPUGeometry::getViewMatrix() {
    return m_ViewMatrix;
}

void GPUGeometry::setViewMatrix(const glm::mat4& viewMatrix) {
    m_ViewMatrix = viewMatrix;
}

glm::mat4 GPUGeometry::getProjectionMatrix() {
    return m_ProjectionMatrix;
}

// Static factory method
std::shared_ptr<GPUGeometry> GPUGeometry::create(
    std::shared_ptr<gpu::CollisionManager> collision,
    GPUGeometryType geomType)
{
    // Get the last group
    int groupIdx = collision->getNumGroups() - 1;
    if (groupIdx < 0) {
        throw std::runtime_error("GPUGeometry::create() - No bodies in collision manager!");
    }

    gpu::BodyGroup group = collision->getGroup(groupIdx);

    auto gpuGeom = std::shared_ptr<GPUGeometry>(
        new GPUGeometry(collision, group.startIndex, group.count)
    );

    switch (geomType) {
    case GPUGeometryType::Cube: {
        auto cube = std::make_shared<Cube>();
        gpuGeom->setPrimitive(cube);
        gpuGeom->m_vs_path = getAssetPath("resources/cube_instanced_vs.glsl");
        gpuGeom->m_fs_path = getAssetPath("resources/cube_fs.glsl");
        break;
    }
    case GPUGeometryType::Sphere: {
        gpuGeom->setPrimitive(std::make_shared<Sphere>(1.0f, 36, 18));
        gpuGeom->m_vs_path = getAssetPath("resources/sphere_instanced_vs.glsl");
        gpuGeom->m_fs_path = getAssetPath("resources/sphere_fs.glsl");
        break;
    }
    case GPUGeometryType::Plane: {
        throw std::runtime_error("Plane geometry not yet implemented");
        break;
    }
    default:
        throw std::runtime_error("Unknown geometry type");
    }

    return gpuGeom;
}