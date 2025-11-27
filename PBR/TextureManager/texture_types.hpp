// texture_types.hpp
#pragma once

#if defined(__CUDACC__)
#include <cuda_runtime.h>
using TextureDeviceObject = cudaTextureObject_t;
#define TEXTURE_BACKEND_CUDA

#elif defined(_FUNGT_USE_SYCL_)
#include <sycl/sycl.hpp>
// SYCL textures are more complex - typically need both image and sampler
struct TextureDeviceObject {
    sycl::image<2>* image;
    sycl::sampler* sampler;
    };
#define TEXTURE_BACKEND_SYCL

#else
    // Fallback for CPU or unsupported platforms
using TextureDeviceObject = void*;
#define TEXTURE_BACKEND_NONE
#endif