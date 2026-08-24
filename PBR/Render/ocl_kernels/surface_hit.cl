typedef struct fgt_surface_hit {
    float distance;
    float3 point;
    float3 geometric_normal;
    float3 shading_normal;
    float2 uv;
    fgt_material_data material;
    int triangle_index;
} fgt_surface_hit;

inline float3 fgt_storage_vec3_to_float3(fgt_vec3 value)
{
    return (float3)(value.x, value.y, value.z);
}

inline float2 fgt_storage_vec2_to_float2(fgt_vec2 value)
{
    return (float2)(value.x, value.y);
}

inline bool fgt_resolve_surface_hit(
    fgt_geometry_hit geometry_hit,
    __global const fgt_triangle_shading* triangle_shading,
    __global const fgt_material_data* materials,
    int num_triangles,
    int num_materials,
    __private fgt_surface_hit* surface_hit)
{
    const int triangle_index = geometry_hit.triangle_index;
    if (triangle_index < 0 || triangle_index >= num_triangles) {
        return false;
    }

    const fgt_triangle_shading shading =
        triangle_shading[triangle_index];
    if (shading.material_index < 0 ||
        shading.material_index >= num_materials) {
        return false;
    }

    const float barycentric_x = geometry_hit.barycentric.x;
    const float barycentric_y = geometry_hit.barycentric.y;
    const float barycentric_z = geometry_hit.barycentric.z;

    const float3 normal0 = fgt_storage_vec3_to_float3(shading.n0);
    const float3 normal1 = fgt_storage_vec3_to_float3(shading.n1);
    const float3 normal2 = fgt_storage_vec3_to_float3(shading.n2);
    const float3 interpolated_normal =
        normal0 * barycentric_x +
        normal1 * barycentric_y +
        normal2 * barycentric_z;

    const float3 geometric_normal = geometry_hit.geometric_normal;
    float3 shading_normal = geometric_normal;
    if (dot(interpolated_normal, interpolated_normal) > 1.0e-12f) {
        shading_normal = normalize(interpolated_normal);
    }
    if (dot(shading_normal, geometric_normal) < 0.0f) {
        shading_normal = -shading_normal;
    }

    const float2 uv0 = fgt_storage_vec2_to_float2(shading.uv0);
    const float2 uv1 = fgt_storage_vec2_to_float2(shading.uv1);
    const float2 uv2 = fgt_storage_vec2_to_float2(shading.uv2);

    surface_hit->distance = geometry_hit.distance;
    surface_hit->point = geometry_hit.point;
    surface_hit->geometric_normal = geometric_normal;
    surface_hit->shading_normal = shading_normal;
    surface_hit->uv =
        uv0 * barycentric_x +
        uv1 * barycentric_y +
        uv2 * barycentric_z;
    surface_hit->material = materials[shading.material_index];
    surface_hit->triangle_index = triangle_index;
    return true;
}
