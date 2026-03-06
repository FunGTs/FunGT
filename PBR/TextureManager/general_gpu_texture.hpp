#if !defined(_GENERAL_GPU_BUFFER_H_)
#define _GENERAL_GPU_BUFFER_H_

struct GPUTexture {
    float* data;      // Device pointer to RGBA float data
    int width;
    int height;
};

#endif // _GENERAL_GPU_BUFFER_H_
