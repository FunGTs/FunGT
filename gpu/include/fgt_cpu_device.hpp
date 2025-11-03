#pragma once

// --- Choose the backend ---
#if defined(FUNGT_USE_SYCL)
#define __KERNEL_SYCL__
#elif defined(__CUDACC__)
#define __KERNEL_CUDA__
#elif defined(FUNGT_USE_HIP)
#define __KERNEL_HIP__
#else
#define __KERNEL_CPU__
#endif

// --- Common qualifiers ---
#define fgt_restrict __restrict__
#define fgt_align(n) __attribute__((aligned(n)))
#define fgt_always_inline __attribute__((always_inline))
#define fgt_noinline __attribute__((noinline))
#define fgt_inline inline

// --- Backend-specific mappings ---
#if defined(__KERNEL_CUDA__)
#define fgt_device __host__ __device__
#define fgt_device_forceinline __host__ __device__ __forceinline__
#define fgt_device_constant __constant__
#define fgt_global __global__
#define fgt_shared __shared__

#elif defined(__KERNEL_SYCL__)
#define fgt_device inline
#define fgt_device_forceinline inline
#define fgt_device_constant const
#define fgt_global
#define fgt_shared /* use local_accessor in SYCL kernels */

#elif defined(__KERNEL_CPU__)
#define fgt_device inline              // ← Changed from empty to inline
#define fgt_device_forceinline inline  // ← Already correct
#define fgt_device_constant static const
#define fgt_global
#define fgt_shared
#endif