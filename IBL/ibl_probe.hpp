#if !defined(_IBL_PROBE_HPP_)
#define _IBL_PROBE_HPP_

#include <memory>
#include <string>
#include "../Shaders/shader.hpp"
#include "../GraphicsRenderBackend/gpu_texture.hpp"
#include "../GraphicsRenderBackend/gpu_buffer.hpp"
#include "../Textures/textures.hpp"
#include "../include/glmath.hpp"

struct IBLCubeVertex {
    glm::vec3 position;
    FUNGT_VERTEX_FORMAT(IBLCubeVertex, position)
};

// Builds an irradiance cubemap from an equirectangular HDR file:
// 1. Loads the HDR as a 2D float texture.
// 2. Renders it onto the 6 faces of a cubemap (equirect -> cubemap).
// 3. Convolves that cubemap into a small diffuse irradiance cubemap.
class IBLProbe {
public:
    IBLProbe();
    ~IBLProbe();

    void build(const std::string& hdrPath);

    unsigned int getIrradianceMapID() const { return m_irradianceMap.getID(); }

private:
    Texture m_envCubemap{ TextureType::CubeMap };
    Texture m_irradianceMap{ TextureType::CubeMap };

    std::unique_ptr<GPUBuffer> m_cubeBuffer;

    void buildCubeMesh();
    void renderCube();

    void renderEquirectToCubemap(unsigned int equirectTexID, unsigned int cubemapID, int faceSize);
    void convolveIrradiance(unsigned int envCubemapID, unsigned int irradianceCubemapID, int faceSize);
};

#endif // _IBL_PROBE_HPP_
