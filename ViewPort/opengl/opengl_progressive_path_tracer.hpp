#ifndef OPENGL_PROGRESSIVE_PATH_TRACER_HPP
#define OPENGL_PROGRESSIVE_PATH_TRACER_HPP

#include "../progressive_path_tracer_viewport.hpp"
#include "../../include/prerequisites.hpp"

class OpenGLProgressivePathTracer : public ProgressivePathTracer {
public:
    OpenGLProgressivePathTracer() = default;
    ~OpenGLProgressivePathTracer() = default;

    void renderSample(int sample, uint32_t targetTexture) override;
    void renderSampleInterop(int sample, uint32_t targetPBO) override;
};

#endif // OPENGL_PROGRESSIVE_PATH_TRACER_HPP
