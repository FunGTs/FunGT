#include "opengl_progressive_path_tracer.hpp"
#include "PBR/Space/space.hpp"
#include <cmath>
#include <vector>

void OpenGLProgressivePathTracer::renderSample(int sample, uint32_t targetTexture)
{
    if (!m_initialized || !m_space) {
        std::cerr << "Path tracer not initialized!" << std::endl;
        return;
    }

    m_space->setSamples(1);
    auto framebuffer = m_space->Render(m_width, m_height, sample);

    for (int i = 0; i < m_width * m_height; i++) {
        int bufIdx = i * 4;
        m_accumBuffer[bufIdx + 0] += framebuffer[i].x;
        m_accumBuffer[bufIdx + 1] += framebuffer[i].y;
        m_accumBuffer[bufIdx + 2] += framebuffer[i].z;
        m_accumBuffer[bufIdx + 3] = 1.0f;
    }

    float invSamples = 1.0f / (sample + 1);
    std::vector<float> gammaCorrected(m_width * m_height * 4);

    for (int i = 0; i < m_width * m_height; i++) {
        int idx = i * 4;
        float r = m_accumBuffer[idx + 0] * invSamples;
        float g = m_accumBuffer[idx + 1] * invSamples;
        float b = m_accumBuffer[idx + 2] * invSamples;
        gammaCorrected[idx + 0] = std::pow(std::clamp(r, 0.0f, 1.0f), 1.0f / 2.2f);
        gammaCorrected[idx + 1] = std::pow(std::clamp(g, 0.0f, 1.0f), 1.0f / 2.2f);
        gammaCorrected[idx + 2] = std::pow(std::clamp(b, 0.0f, 1.0f), 1.0f / 2.2f);
        gammaCorrected[idx + 3] = 1.0f;
    }

    glBindTexture(GL_TEXTURE_2D, targetTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height,
        GL_RGBA, GL_FLOAT, gammaCorrected.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLProgressivePathTracer::renderSampleInterop(int sample, uint32_t targetPBO)
{
    if (!m_initialized || !m_space) {
        std::cerr << "Path tracer not initialized!" << std::endl;
        return;
    }

    m_space->setSamples(1);
    m_space->RenderOpenGLInterop(
        m_width,
        m_height,
        sample,
        targetPBO);
}
