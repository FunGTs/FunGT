#include "simple_geometry.hpp"


SimpleGeometry::SimpleGeometry() {
    // Private constructor for factory pattern
}

SimpleGeometry::~SimpleGeometry() {
    // Destructor
}

void SimpleGeometry::setPrimitive(std::shared_ptr<Primitive> primitive) {
    m_primitive = primitive;
}

void SimpleGeometry::setShaderPaths(const std::string &vs_path, const std::string &fs_path) {
    m_vs_path = vs_path;
    m_fs_path = fs_path;
}

void SimpleGeometry::load(const std::string &pathToTexture) {
    if (!m_primitive) {
        throw std::runtime_error("SimpleGeometry::load() - No primitive set! Call setPrimitive() first.");
    }

    m_primitive->setData();
    m_Shader.create(m_vs_path, m_fs_path);
    m_primitive->setTexture(pathToTexture);
    m_primitive->InitGraphics();
}

void SimpleGeometry::position(float x, float y, float z) {
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
    m_ModelMatrix = glm::translate(m_ModelMatrix, m_position);
}

void SimpleGeometry::rotation(float x, float y, float z) {
    m_rotation.x = x;
    m_rotation.y = y;
    m_rotation.z = z;
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.x), glm::vec3(1.f, 0.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.y), glm::vec3(0.f, 1.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.z), glm::vec3(0.f, 0.f, 1.f));
}

void SimpleGeometry::scale(float s) {
    m_scale = glm::vec3(s);
    m_ModelMatrix = glm::scale(m_ModelMatrix, m_scale);
}

void SimpleGeometry::draw() {
    if (m_primitive) {
        m_primitive->draw();
    }
}

Shader& SimpleGeometry::getShader() {
    return m_Shader;
}

glm::mat4 SimpleGeometry::getViewMatrix() {
    return m_ViewMatrix;
}

void SimpleGeometry::setViewMatrix(const glm::mat4 &viewMatrix) {
    m_ViewMatrix = viewMatrix;
}

void SimpleGeometry::updateModelMatrix() {
    float currentTime = glfwGetTime();

    m_ModelMatrix = glm::mat4(1.f);
    m_ModelMatrix = glm::translate(m_ModelMatrix, m_position);
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.x), glm::vec3(1.f, 0.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.y), glm::vec3(0.f, 1.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.z), glm::vec3(0.f, 0.f, 1.f));
    m_ModelMatrix = glm::scale(m_ModelMatrix, m_scale);
}

glm::mat4 SimpleGeometry::getProjectionMatrix() {
    return m_ProjectionMatrix;
}

glm::mat4 SimpleGeometry::getModelMatrix() {
    return m_ModelMatrix;
}

// Static factory method
std::shared_ptr<SimpleGeometry> SimpleGeometry::create(Geometry geomType) {
    auto simpleGeom = std::shared_ptr<SimpleGeometry>(new SimpleGeometry());

    switch (geomType) {
        case Geometry::Cube: {
            auto cube = std::make_shared<Cube>();
            simpleGeom->setPrimitive(cube);
            simpleGeom->m_vs_path = getAssetPath("resources/cube.vs");
            simpleGeom->m_fs_path = getAssetPath("resources/cube.fs");
            break;
        }
        case Geometry::Sphere: {
            // TODO: Implement Sphere primitive
            simpleGeom->setPrimitive(std::make_shared<geometry::Sphere>(1.0f, 72, 18));
            simpleGeom->m_vs_path = getAssetPath("resources/sphere_vs.glsl");
            simpleGeom->m_fs_path = getAssetPath("resources/sphere_fs.glsl");
            break;
        }
        case Geometry::Box: {
            // TODO: Implement Plane primitive
            simpleGeom->setPrimitive(std::make_shared<geometry::Box>(20.0f, 1.0f, 20.0f));
            simpleGeom->m_vs_path = getAssetPath("resources/box_vs.glsl");
            simpleGeom->m_fs_path = getAssetPath("resources/box_fs.glsl");
            break;
        }
        default:
            throw std::runtime_error("Unknown geometry type");
    }

    return simpleGeom;
}
