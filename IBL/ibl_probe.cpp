#include "ibl_probe.hpp"
#include "../Path_Manager/path_manager.hpp"
#include "../Textures/textures.hpp"
#include "../include/prerequisites.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <stdexcept>

namespace {

// Capture passes must not depend on the viewport's current raster state.
struct IBLCaptureState {
    GLint m_drawFramebuffer;
    GLint m_readFramebuffer;
    GLint m_renderbuffer;
    GLint m_viewport[4];
    GLint m_program;
    GLint m_vertexArray;
    GLint m_activeTexture;
    GLint m_texture2D;
    GLint m_textureCube;
    GLboolean m_colorMask[4];
    GLboolean m_depthTest;
    GLboolean m_cullFace;
    GLboolean m_blend;
    GLboolean m_scissor;
    GLboolean m_srgb;
    GLboolean m_seamless;
    GLint m_polygonMode[2];

    IBLCaptureState()
    {
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_drawFramebuffer);
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_readFramebuffer);
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &m_renderbuffer);
        glGetIntegerv(GL_VIEWPORT, m_viewport);
        glGetIntegerv(GL_CURRENT_PROGRAM, &m_program);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &m_vertexArray);
        glGetIntegerv(GL_ACTIVE_TEXTURE, &m_activeTexture);
        glActiveTexture(GL_TEXTURE0);
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &m_texture2D);
        glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &m_textureCube);
        glGetBooleanv(GL_COLOR_WRITEMASK, m_colorMask);
        glGetIntegerv(GL_POLYGON_MODE, m_polygonMode);
        m_depthTest = glIsEnabled(GL_DEPTH_TEST);
        m_cullFace = glIsEnabled(GL_CULL_FACE);
        m_blend = glIsEnabled(GL_BLEND);
        m_scissor = glIsEnabled(GL_SCISSOR_TEST);
        m_srgb = glIsEnabled(GL_FRAMEBUFFER_SRGB);
        m_seamless = glIsEnabled(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glDisable(GL_SCISSOR_TEST);
        glDisable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    ~IBLCaptureState()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_drawFramebuffer);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_readFramebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
        glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
        glUseProgram(m_program);
        glBindVertexArray(m_vertexArray);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture2D);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureCube);
        glActiveTexture(m_activeTexture);
        glColorMask(m_colorMask[0], m_colorMask[1], m_colorMask[2], m_colorMask[3]);
        glPolygonMode(GL_FRONT_AND_BACK, m_polygonMode[0]);
        restore(GL_DEPTH_TEST, m_depthTest);
        restore(GL_CULL_FACE, m_cullFace);
        restore(GL_BLEND, m_blend);
        restore(GL_SCISSOR_TEST, m_scissor);
        restore(GL_FRAMEBUFFER_SRGB, m_srgb);
        restore(GL_TEXTURE_CUBE_MAP_SEAMLESS, m_seamless);
    }

    static void restore(GLenum capability, GLboolean enabled)
    {
        if (enabled) glEnable(capability);
        else glDisable(capability);
    }
};

}

IBLProbe::IBLProbe()
{
    buildCubeMesh();
}

IBLProbe::~IBLProbe()
{
}

void IBLProbe::buildCubeMesh()
{
    IBLCubeVertex vertices[] = {
        {{-1.0f,  1.0f, -1.0f}},
        {{-1.0f, -1.0f, -1.0f}},
        {{ 1.0f, -1.0f, -1.0f}},
        {{ 1.0f, -1.0f, -1.0f}},
        {{ 1.0f,  1.0f, -1.0f}},
        {{-1.0f,  1.0f, -1.0f}},

        {{-1.0f, -1.0f,  1.0f}},
        {{-1.0f, -1.0f, -1.0f}},
        {{-1.0f,  1.0f, -1.0f}},
        {{-1.0f,  1.0f, -1.0f}},
        {{-1.0f,  1.0f,  1.0f}},
        {{-1.0f, -1.0f,  1.0f}},

        {{ 1.0f, -1.0f, -1.0f}},
        {{ 1.0f, -1.0f,  1.0f}},
        {{ 1.0f,  1.0f,  1.0f}},
        {{ 1.0f,  1.0f,  1.0f}},
        {{ 1.0f,  1.0f, -1.0f}},
        {{ 1.0f, -1.0f, -1.0f}},

        {{-1.0f, -1.0f,  1.0f}},
        {{-1.0f,  1.0f,  1.0f}},
        {{ 1.0f,  1.0f,  1.0f}},
        {{ 1.0f,  1.0f,  1.0f}},
        {{ 1.0f, -1.0f,  1.0f}},
        {{-1.0f, -1.0f,  1.0f}},

        {{-1.0f,  1.0f, -1.0f}},
        {{ 1.0f,  1.0f, -1.0f}},
        {{ 1.0f,  1.0f,  1.0f}},
        {{ 1.0f,  1.0f,  1.0f}},
        {{-1.0f,  1.0f,  1.0f}},
        {{-1.0f,  1.0f, -1.0f}},

        {{-1.0f, -1.0f, -1.0f}},
        {{-1.0f, -1.0f,  1.0f}},
        {{ 1.0f, -1.0f, -1.0f}},
        {{ 1.0f, -1.0f, -1.0f}},
        {{-1.0f, -1.0f,  1.0f}},
        {{ 1.0f, -1.0f,  1.0f}}
    };

    m_cubeBuffer = GPUBuffer::create();
    m_cubeBuffer->genVAO();
    m_cubeBuffer->bindVAO();
    m_cubeBuffer->create(BufferType::Vertex, vertices, sizeof(vertices));
    m_cubeBuffer->applyFormat(IBLCubeVertex::getFormat());
    m_cubeBuffer->unbindVAO();
}

void IBLProbe::renderCube()
{
    m_cubeBuffer->bindVAO();
    m_cubeBuffer->drawArrays(36);
    m_cubeBuffer->unbindVAO();
}

static void buildFaceMatrices(glm::mat4 captureViews[6], glm::mat4& captureProjection)
{
    captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    captureViews[0] = glm::lookAt(glm::vec3(0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    captureViews[1] = glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    captureViews[2] = glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f));
    captureViews[3] = glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f));
    captureViews[4] = glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    captureViews[5] = glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
}

void IBLProbe::renderEquirectToCubemap(unsigned int equirectTexID, unsigned int cubemapID, int faceSize)
{
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, faceSize, faceSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    auto shader = Shader::create();
    shader->create(getAssetPath("resources/equirect_to_cubemap.vs"),
                   getAssetPath("resources/equirect_to_cubemap.fs"));

    glm::mat4 captureViews[6];
    glm::mat4 captureProjection;
    buildFaceMatrices(captureViews, captureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, equirectTexID);

    shader->Bind();
    shader->set1i(0, "equirectMap");

    glViewport(0, 0, faceSize, faceSize);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; i++) {
        shader->setMat4fv(captureViews[i], "ViewMatrix");
        shader->setMat4fv(captureProjection, "ProjectionMatrix");

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemapID, 0);

        GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "IBLProbe: equirect->cubemap FBO incomplete on face " << i
                       << ", status=0x" << std::hex << fboStatus << std::dec << std::endl;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);
}

void IBLProbe::convolveIrradiance(unsigned int envCubemapID, unsigned int irradianceCubemapID, int faceSize)
{
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, faceSize, faceSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    auto shader = Shader::create();
    shader->create(getAssetPath("resources/irradiance_convolve.vs"),
                   getAssetPath("resources/irradiance_convolve.fs"));

    glm::mat4 captureViews[6];
    glm::mat4 captureProjection;
    buildFaceMatrices(captureViews, captureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemapID);

    shader->Bind();
    shader->set1i(0, "environmentMap");

    GLint convolutionProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &convolutionProgram);
    assert(convolutionProgram != 0);

    glViewport(0, 0, faceSize, faceSize);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, faceSize, faceSize);
    for (unsigned int i = 0; i < 6; i++) {
        shader->setMat4fv(captureViews[i], "ViewMatrix");
        shader->setMat4fv(captureProjection, "ProjectionMatrix");

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceCubemapID, 0);

        GLint attachedTexture;
        GLint currentProgram;
        GLint environmentTexture;
        glGetFramebufferAttachmentParameteriv(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
            &attachedTexture);
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        glActiveTexture(GL_TEXTURE0);
        glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &environmentTexture);

        assert(attachedTexture == static_cast<GLint>(irradianceCubemapID));
        assert(currentProgram == convolutionProgram);
        assert(environmentTexture == static_cast<GLint>(envCubemapID));

        GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "IBLProbe: irradiance convolve FBO incomplete on face " << i
                       << ", status=0x" << std::hex << fboStatus << std::dec << std::endl;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);
}

void IBLProbe::prefilterSpecular()
{
    const int faceSize = 128;
    const int mipCount = 8;
    m_prefilterMap.allocateEmptyCubemap(faceSize, true);

    auto shader = Shader::create();
    shader->create(getAssetPath("resources/irradiance_convolve.vs"),
                   getAssetPath("resources/specular_prefilter.fs"));

    glm::mat4 captureViews[6];
    glm::mat4 captureProjection;
    buildFaceMatrices(captureViews, captureProjection);

    unsigned int captureFBO;
    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap.getID());
    shader->Bind();
    shader->set1i(0, "environmentMap");
    shader->setMat4fv(captureProjection, "ProjectionMatrix");

    for (int mip = 0; mip < mipCount; ++mip) {
        const int mipSize = faceSize >> mip;
        glViewport(0, 0, mipSize, mipSize);
        shader->setUniform1f(float(mip) / float(mipCount - 1), "roughness");

        for (unsigned int face = 0; face < 6; ++face) {
            shader->setMat4fv(captureViews[face], "ViewMatrix");
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_prefilterMap.getID(), mip);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                glDeleteFramebuffers(1, &captureFBO);
                throw std::runtime_error("IBL specular framebuffer is incomplete");
            }
            renderCube();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);
}

void IBLProbe::buildBRDFLUT()
{
    if (m_brdfLUT.getID() != 0) {
        return;
    }

    const int size = 256;
    m_brdfLUT.allocateEmptyFloat2D(size, size);
    auto shader = Shader::create();
    shader->create(getAssetPath("resources/brdf_integrate.vs"),
                   getAssetPath("resources/brdf_integrate.fs"));

    unsigned int captureFBO;
    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                          GL_TEXTURE_2D, m_brdfLUT.getID(), 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glDeleteFramebuffers(1, &captureFBO);
        throw std::runtime_error("IBL BRDF framebuffer is incomplete");
    }

    glViewport(0, 0, size, size);
    shader->Bind();
    // The vertex shader generates a full-screen triangle using gl_VertexID.
    m_cubeBuffer->bindVAO();
    m_cubeBuffer->drawArrays(3);
    m_cubeBuffer->unbindVAO();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);
}

void IBLProbe::bindIrradianceMap()
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap.getID());
}

void IBLProbe::bindPrefilterMap()
{
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilterMap.getID());
}

void IBLProbe::bindBRDFLUT()
{
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, m_brdfLUT.getID());
}



void IBLProbe::build(const std::string& hdrPath)
{
    IBLCaptureState captureState;
    Texture equirectTexture(TextureType::Texture2D);
    equirectTexture.genTextureHDR(hdrPath);

    const int envFaceSize = 512;
    const int irradianceFaceSize = 128;

    m_envCubemap.allocateEmptyCubemap(envFaceSize, true);
    renderEquirectToCubemap(equirectTexture.getID(), m_envCubemap.getID(), envFaceSize);

    m_irradianceMap.allocateEmptyCubemap(irradianceFaceSize, false);
    convolveIrradiance(m_envCubemap.getID(), m_irradianceMap.getID(), irradianceFaceSize);

    prefilterSpecular();
    buildBRDFLUT();

    std::cout << "IBLProbe: built env cubemap (" << envFaceSize << "px) and irradiance map ("
              << irradianceFaceSize << "px) from " << hdrPath << std::endl;
}
