#include "inf_grid.hpp"
#include "../GraphicsRenderBackend/graphics_render_device.hpp"
#include <iostream>

InfiniteGrid::InfiniteGrid()
    : m_nearPlane(0.1f)
    , m_farPlane(500.0f)
    , m_viewMatrix(glm::mat4(1.0f))
    , m_projectionMatrix(glm::mat4(1.0f))
{
    // Empty VAO — shader generates vertices procedurally
    m_buffer = GPUBuffer::create();
    m_buffer->genVAO();
    std::cout << "InfiniteGrid constructor" << std::endl;
}

InfiniteGrid::~InfiniteGrid() {
    std::cout << "InfiniteGrid destructor" << std::endl;
}

void InfiniteGrid::init(const std::string& vsPath, const std::string& fsPath) {
    m_shader.create(vsPath, fsPath);
    std::cout << "Infinite grid shader created" << std::endl;
}

void InfiniteGrid::draw() {
    //m_shader.Bind();

    // Set uniforms
    m_shader.setUniformMat4fv("ViewMatrix", m_viewMatrix);
    m_shader.setUniformMat4fv("ProjectionMatrix", m_projectionMatrix);
    m_shader.setUniform1f(m_nearPlane, "nearPlane");
    m_shader.setUniform1f(m_farPlane, "farPlane");

    m_buffer->bindVAO();

    auto* dev = GraphicsRenderDevice::Get();
    dev->setBlendFunc(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha);
    dev->setDepthMask(false);

    m_buffer->drawArrays(6);

    dev->setDepthMask(true);

    m_buffer->unbindVAO();
    //m_shader.unBind();
}