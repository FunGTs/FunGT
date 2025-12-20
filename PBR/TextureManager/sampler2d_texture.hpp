#if !defined(_SAMPLER_2D_TEXTURE_HPP_)
#define _SAMPLER_2D_TEXTURE_HPP_
#include "gpu/include/fgt_cpu_device.hpp"
#include "texture_types.hpp"
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

#endif // _SAMPLER_2D_TEXTURE_HPP_
