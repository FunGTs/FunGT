// texture_types.hpp
#pragma once

#if defined(__CUDACC__) || defined(__NVCC__)
#include <cuda_runtime.h>
using TextureDeviceObject = cudaTextureObject_t;
#define TEXTURE_BACKEND_CUDA

#elif defined(FUNGT_USE_SYCL) && !defined(__CUDACC__)
#include <sycl/sycl.hpp>
#include <sycl/ext/oneapi/bindless_images.hpp>  // ← ADD THIS!
namespace syclexp = sycl::ext::oneapi::experimental;           // ← FIX THIS!
using TextureDeviceObject = syclexp::sampled_image_handle;
#define TEXTURE_BACKEND_SYCL

#else
    // Fallback for CPU or unsupported platforms
using TextureDeviceObject = void*;
#define TEXTURE_BACKEND_NONE
#endif