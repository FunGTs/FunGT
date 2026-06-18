#if !defined(_SAMPLER_2D_TEXTURE_HPP_)
#define _SAMPLER_2D_TEXTURE_HPP_
#include "gpu/include/fgt_cpu_device.hpp"
#include "texture_types.hpp"
#include "general_gpu_texture.hpp"
// Portable sampling function
fgt_device_gpu inline fungt::Vec3 sampleTexture2D(
    const TextureDeviceObject& texture,
    float u,
    float v
) {
#ifdef TEXTURE_BACKEND_CUDA
    float4 c = tex2D<float4>(texture, u, v);
    return fungt::Vec3(c.x, c.y, c.z);

#elif defined(TEXTURE_BACKEND_SYCL)
    sycl::float4 c = sycl::ext::oneapi::experimental::sample_image<sycl::float4>(
        texture,
        sycl::float2(u, v)
    );
    return fungt::Vec3(c.x(), c.y(), c.z());

#else
    // CPU fallback or error
    return fungt::Vec3(1.0f, 0.0f, 1.0f); // Magenta error color
#endif
}
// Buffer path - manual bilinear filter for devices without bindless image support
fgt_device_gpu inline fungt::Vec3 sampleTexture2D(
    const GPUTexture& texture,
    float u,
    float v
) {
    int w = texture.width;
    int h = texture.height;

    u = u < 0.0f ? 0.0f : (u > 1.0f ? 1.0f : u);
    v = v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);

    float fx = u * (w - 1);
    float fy = v * (h - 1);

    int x0 = (int)fx;
    int y0 = (int)fy;
    int x1 = x0 + 1 < w ? x0 + 1 : x0;
    int y1 = y0 + 1 < h ? y0 + 1 : y0;

    float tx = fx - (float)x0;
    float ty = fy - (float)y0;

    const float* d = texture.data;
#define FETCH(x, y, c) d[((y) * w + (x)) * 4 + (c)]

    float r = (1.0f - tx) * (1.0f - ty) * FETCH(x0, y0, 0)
        + tx * (1.0f - ty) * FETCH(x1, y0, 0)
        + (1.0f - tx) * ty * FETCH(x0, y1, 0)
        + tx * ty * FETCH(x1, y1, 0);

    float g = (1.0f - tx) * (1.0f - ty) * FETCH(x0, y0, 1)
        + tx * (1.0f - ty) * FETCH(x1, y0, 1)
        + (1.0f - tx) * ty * FETCH(x0, y1, 1)
        + tx * ty * FETCH(x1, y1, 1);

    float b = (1.0f - tx) * (1.0f - ty) * FETCH(x0, y0, 2)
        + tx * (1.0f - ty) * FETCH(x1, y0, 2)
        + (1.0f - tx) * ty * FETCH(x0, y1, 2)
        + tx * ty * FETCH(x1, y1, 2);

#undef FETCH

    return fungt::Vec3(r, g, b);
}


#endif // _SAMPLER_2D_TEXTURE_HPP_
