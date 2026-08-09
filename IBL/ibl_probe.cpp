#include "ibl_probe.hpp"
#include "../Path_Manager/path_manager.hpp"
#include "../Textures/textures.hpp"
#include "../include/prerequisites.hpp"
#include <iostream>
#include <vector>

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

    glViewport(0, 0, faceSize, faceSize);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, faceSize, faceSize);
    for (unsigned int i = 0; i < 6; i++) {
        shader->setMat4fv(captureViews[i], "ViewMatrix");
        shader->setMat4fv(captureProjection, "ProjectionMatrix");

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceCubemapID, 0);

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

void IBLProbe::build(const std::string& hdrPath)
{
    Texture equirectTexture(TextureType::Texture2D);
    equirectTexture.genTextureHDR(hdrPath);

    const int envFaceSize = 512;
    const int irradianceFaceSize = 32;

    m_envCubemap.allocateEmptyCubemap(envFaceSize, true);
    renderEquirectToCubemap(equirectTexture.getID(), m_envCubemap.getID(), envFaceSize);

    m_irradianceMap.allocateEmptyCubemap(irradianceFaceSize, false);
    convolveIrradiance(m_envCubemap.getID(), m_irradianceMap.getID(), irradianceFaceSize);

    std::cout << "IBLProbe: built env cubemap (" << envFaceSize << "px) and irradiance map ("
              << irradianceFaceSize << "px) from " << hdrPath << std::endl;
}
