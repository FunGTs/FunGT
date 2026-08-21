typedef struct fgt_ray {
    float3 origin;
    float3 direction;
} fgt_ray;

inline float3 fgt_vec4_xyz(fgt_vec4 value)
{
    return (float3)(value.x, value.y, value.z);
}

inline fgt_ray fgt_camera_get_ray(
    __constant const fgt_rayspace_camera* camera,
    float image_u,
    float image_v)
{
    const float3 origin = fgt_vec4_xyz(camera->origin_lens_radius);
    const float3 lower_left = fgt_vec4_xyz(camera->lower_left_corner);
    const float3 horizontal = fgt_vec4_xyz(camera->horizontal);
    const float3 vertical = fgt_vec4_xyz(camera->vertical);

    fgt_ray ray;
    ray.origin = origin;
    ray.direction = normalize(
        lower_left + image_u * horizontal + image_v * vertical - origin);
    return ray;
}
