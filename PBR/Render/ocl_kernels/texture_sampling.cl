float4 __builtin_IB_OCL_2d_ld_ro(long image_id, int2 coord, int lod);

inline float3 fgt_sample_bindless_texture(
    __global const ulong* texture_handles,
    __global const int2* texture_dimensions,
    int texture_index,
    float2 uv)
{
    const long handle = (long)texture_handles[texture_index];
    const int2 size = texture_dimensions[texture_index];
    const float2 repeated_uv = uv - floor(uv);
    const int2 coordinate = min(
        convert_int2(repeated_uv * convert_float2(size)),
        size - 1);

    return __builtin_IB_OCL_2d_ld_ro(
        handle,
        coordinate,
        0).xyz;
}
