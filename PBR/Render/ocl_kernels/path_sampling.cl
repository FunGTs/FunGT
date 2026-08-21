inline float3 fgt_sample_hemisphere(
    float3 normal,
    __private fgt_rng* rng)
{
    const float u = fgt_rng_next_float(rng);
    const float v = fgt_rng_next_float(rng);
    const float z = sqrt(1.0f - u);
    const float radius = sqrt(u);
    const float phi = 2.0f * FGT_PI_F * v;

    const float x = radius * cos(phi);
    const float y = radius * sin(phi);
    const float3 helper = fabs(normal.x) > 0.1f
        ? (float3)(0.0f, 1.0f, 0.0f)
        : (float3)(1.0f, 0.0f, 0.0f);
    const float3 tangent = normalize(cross(helper, normal));
    const float3 bitangent = cross(normal, tangent);

    return normalize(tangent * x + bitangent * y + normal * z);
}

inline bool fgt_sample_emissive_triangle(
    __global const fgt_triangle_geom* triangle_geometry,
    __global const fgt_triangle_shading* triangle_shading,
    __global const fgt_material_data* materials,
    __global const fgt_int32* emissive_triangles,
    int num_triangles,
    int num_materials,
    int num_emissive_triangles,
    __private fgt_rng* rng,
    __private float3* light_position,
    __private float3* light_normal,
    __private float3* light_emission,
    __private float* light_pdf)
{
    if (num_emissive_triangles <= 0) {
        return false;
    }

    const uint random_index =
        fgt_rng_next_u32(rng) % (uint)num_emissive_triangles;
    const int triangle_index = emissive_triangles[random_index];
    if (triangle_index < 0 || triangle_index >= num_triangles) {
        return false;
    }

    const fgt_triangle_geom geometry = triangle_geometry[triangle_index];
    const fgt_triangle_shading shading = triangle_shading[triangle_index];
    if (shading.material_index < 0 ||
        shading.material_index >= num_materials) {
        return false;
    }

    float barycentric_y = fgt_rng_next_float(rng);
    float barycentric_z = fgt_rng_next_float(rng);
    if (barycentric_y + barycentric_z > 1.0f) {
        barycentric_y = 1.0f - barycentric_y;
        barycentric_z = 1.0f - barycentric_z;
    }
    const float barycentric_x =
        1.0f - barycentric_y - barycentric_z;

    const float3 v0 = (float3)(
        geometry.v0.x, geometry.v0.y, geometry.v0.z);
    const float3 edge1 = (float3)(
        geometry.edge1.x, geometry.edge1.y, geometry.edge1.z);
    const float3 edge2 = (float3)(
        geometry.edge2.x, geometry.edge2.y, geometry.edge2.z);
    const float3 normal0 = fgt_storage_vec3_to_float3(shading.n0);
    const float3 normal1 = fgt_storage_vec3_to_float3(shading.n1);
    const float3 normal2 = fgt_storage_vec3_to_float3(shading.n2);
    const float area = 0.5f * length(cross(edge1, edge2));
    if (area < 1.0e-8f) {
        return false;
    }

    const fgt_material_data material = materials[shading.material_index];
    *light_position = v0 + edge1 * barycentric_y + edge2 * barycentric_z;
    *light_normal = normalize(
        normal0 * barycentric_x +
        normal1 * barycentric_y +
        normal2 * barycentric_z);
    *light_emission = (float3)(
        material.base_color[0],
        material.base_color[1],
        material.base_color[2]) * material.emission;
    *light_pdf = 1.0f / ((float)num_emissive_triangles * area);
    return true;
}
