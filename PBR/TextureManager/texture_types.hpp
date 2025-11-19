// texture_types.hpp
#pragma once

#ifdef _CUDACC_
using CUDATextureType = cudaTextureObject_t;
#else
    // Dummy type when CUDA not available
//using CUDATextureType = cudaTextureObject_t;
#endif
// #include<vector>
// // CPU texture is always available
// struct CPUTexture {
//     std::vector<unsigned char> data;
//     int width;
//     int height;
//     int channels;
//     // ... sample() method
// };

#ifdef _FUNGT_USE_SYCL_
// SYCL texture type when available
using SYCLTextureType = /* SYCL texture type */;
#else
using SYCLTextureType = void*;
#endif