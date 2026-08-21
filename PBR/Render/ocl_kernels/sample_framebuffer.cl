__kernel void sample_framebuffer(
    __global float4* framebuffer,
    int samples_per_pixel){
    int idx = get_global_id(0);
    const float inverse_samples = 1.0f / (float)samples_per_pixel;
    float4 pixel = framebuffer[idx];
    pixel.xyz *= inverse_samples;
    pixel.w = 1.0f;
    framebuffer[idx] = pixel;
}

__kernel void present_framebuffer(
    __global const float4* src_buffer,
    __global float4* dst_buffer,
    int accumulated_samples)
{
    const int idx = get_global_id(0);
    const float inv = 1.0f / (float)accumulated_samples;
    const float4 pixel = src_buffer[idx];
    const float gamma = 1.0f / 2.2f;
    float r = clamp(pixel.x * inv, 0.0f, 1.0f);
    float g = clamp(pixel.y * inv, 0.0f, 1.0f);
    float b = clamp(pixel.z * inv, 0.0f, 1.0f);
    dst_buffer[idx] = (float4)(pow(r, gamma), pow(g, gamma), pow(b, gamma), 1.0f);
}
