__kernel void sample_framebuffer(
    __global fgt_vec4* framebuffer,
    int samples_per_pixel){
    int idx = get_global_id(0);
    const float inverse_samples = 1.0f / (float)samples_per_pixel;
    framebuffer[idx].x *= inverse_samples;
    framebuffer[idx].y *= inverse_samples;
    framebuffer[idx].z *= inverse_samples;
    framebuffer[idx].w = 1.0f;

}
