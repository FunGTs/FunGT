#if !defined(_CUDA_RENDER_H_)
#define _CUDA_RENDER_H_
// Define FUNGT_USE_CUDA FIRST, before ANY includes
#ifdef __CUDACC__
#define FUNGT_USE_CUDA
#include <cuda_runtime.h>
#include <curand_kernel.h>
#include <device_launch_parameters.h>
#endif

// NOW include your headers - they'll see FUNGT_USE_CUDA
#include <iostream>
#include "icompute_renderer.hpp"
#include "../../../gpu/include/fgt_cpu_device.hpp"
#include "../../Intersection/intersection.hpp"
#include "../../Ray/ray.hpp"


#define CUDA_CHECK(err) do { cudaError_t e = (err); if (e != cudaSuccess) { \
    std::cerr << "CUDA error: " << cudaGetErrorString(e) << " at " << __FILE__ << ":" << __LINE__ << std::endl; exit(1); }} while(0)

class CUDA_Renderer : public IComputeRenderer{
    
    std::vector<fungt::Vec3> RenderScene(
        int width, int height,
        const std::vector<Triangle>& triangleList,
        const PBRCamera& camera,
        int samplesPerPixel
    );



};




#endif // _CUDA_RENDER_H_
