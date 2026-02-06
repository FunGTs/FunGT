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

   // m_shader.Bind();

    auto kernel = m_collision->getKernel();
    if (kernel == nullptr) {
        std::cout << "Kernel is invalid" << std::endl;
        return;
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, kernel->getModelMatrixSSBO());
    // tells shader which index to start from
    m_shader.setUniform1i("u_startIndex", m_startIndex);

    m_primitive->IntancedDraw(m_shader, m_instanceCount);
   
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
std::shared_ptr<GPUGeometry> GPUGeometry::create(
    std::shared_ptr<gpu::CollisionManager> collision,
    GPUGeometryType geomType,
    std::shared_ptr<Primitive> primitive)
{
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
        std::cout << "Creating a cube" << std::endl;
        if (primitive != nullptr) {
            gpuGeom->setPrimitive(primitive);
        }
        else {
            gpuGeom->setPrimitive(std::make_shared<Cube>());
        }
        gpuGeom->m_vs_path = getAssetPath("resources/cube_instanced_vs.glsl");
        gpuGeom->m_fs_path = getAssetPath("resources/cube_instanced_fs.glsl");
        break;
    }
    case GPUGeometryType::Sphere: {
        if (primitive != nullptr) {
            gpuGeom->setPrimitive(primitive);
        }
        else {
            gpuGeom->setPrimitive(std::make_shared<geometry::Sphere>(1.0f, 36, 18));
        }
        gpuGeom->m_vs_path = getAssetPath("resources/sphere_instanced_vs.glsl");
        gpuGeom->m_fs_path = getAssetPath("resources/sphere_instanced_fs.glsl");
        break;
    }
    case GPUGeometryType::Box: {
        if (primitive != nullptr) {
            gpuGeom->setPrimitive(primitive);
        }
        else {
            gpuGeom->setPrimitive(std::make_shared<geometry::Box>());
        }
        gpuGeom->m_vs_path = getAssetPath("resources/box_instanced_vs.glsl");
        gpuGeom->m_fs_path = getAssetPath("resources/box_instanced_fs.glsl");
        break;
    }
    case GPUGeometryType::Plane: {
        throw std::runtime_error("Plane geometry not yet implemented");
    }
    default:
        throw std::runtime_error("Unknown geometry type");
    }

    return gpuGeom;
}