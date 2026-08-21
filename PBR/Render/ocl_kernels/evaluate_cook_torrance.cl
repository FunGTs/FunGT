#define FGT_PI_F 3.14159265358979323846f

inline float fgt_distribution_ggx(float normal_dot_half, float roughness)
{
    const float alpha = roughness * roughness;
    const float alpha_squared = alpha * alpha;
    const float normal_dot_half_squared =
        normal_dot_half * normal_dot_half;
    float denominator =
        normal_dot_half_squared * (alpha_squared - 1.0f) + 1.0f;
    denominator = fmax(denominator, 1.0e-6f);

    return alpha_squared /
        (FGT_PI_F * denominator * denominator + 1.0e-8f);
}

inline float fgt_geometry_schlick_ggx(
    float normal_dot_direction,
    float k)
{
    return normal_dot_direction /
        (normal_dot_direction * (1.0f - k) + k + 1.0e-8f);
}

inline float fgt_geometry_smith(
    float normal_dot_view,
    float normal_dot_light,
    float roughness)
{
    const float remapped_roughness = roughness + 1.0f;
    const float k =
        (remapped_roughness * remapped_roughness) / 8.0f;

    return
        fgt_geometry_schlick_ggx(normal_dot_view, k) *
        fgt_geometry_schlick_ggx(normal_dot_light, k);
}

inline float3 fgt_fresnel_schlick(float3 f0, float view_dot_half)
{
    const float value = 1.0f - view_dot_half;
    const float value_squared = value * value;
    const float value_fifth =
        value_squared * value_squared * value;

    return f0 + ((float3)(1.0f) - f0) * value_fifth;
}

inline void fgt_material_to_pbr(
    fgt_material_data material,
    __private float3* base_color,
    __private float* metallic,
    __private float* roughness,
    __private float3* f0)
{
    *base_color = (float3)(
        material.base_color[0],
        material.base_color[1],
        material.base_color[2]);
    *metallic = clamp(material.metallic, 0.0f, 1.0f);
    *roughness = clamp(material.roughness, 0.05f, 1.0f);

    const float3 dielectric_f0 = (float3)(material.reflectance);
    *f0 = mix(dielectric_f0, *base_color, *metallic);
}

inline float3 fgt_evaluate_cook_torrance(
    float3 normal,
    float3 view_direction,
    float3 light_direction,
    fgt_material_data material,
    float3 light_radiance)
{
    float3 base_color;
    float metallic;
    float roughness;
    float3 f0;
    fgt_material_to_pbr(
        material,
        &base_color,
        &metallic,
        &roughness,
        &f0);

    const float3 half_direction =
        normalize(view_direction + light_direction);
    const float normal_dot_light =
        fmax(dot(normal, light_direction), 0.0f);
    const float normal_dot_view =
        fmax(dot(normal, view_direction), 0.0f);
    const float normal_dot_half =
        fmax(dot(normal, half_direction), 0.0f);
    const float view_dot_half =
        fmax(dot(view_direction, half_direction), 0.0f);

    if (normal_dot_light <= 0.0f || normal_dot_view <= 0.0f) {
        return (float3)(0.0f);
    }

    const float distribution = fmin(
        fgt_distribution_ggx(normal_dot_half, roughness),
        100.0f);
    const float geometry = fgt_geometry_smith(
        normal_dot_view,
        normal_dot_light,
        roughness);
    const float3 fresnel = fgt_fresnel_schlick(f0, view_dot_half);

    const float3 numerator = fresnel * (distribution * geometry);
    const float denominator =
        4.0f * normal_dot_view * normal_dot_light + 1.0e-6f;
    const float3 specular = numerator / denominator;

    const float3 diffuse_weight =
        ((float3)(1.0f) - fresnel) * (1.0f - metallic);
    const float3 diffuse = base_color / FGT_PI_F;

    return
        (diffuse_weight * diffuse + specular) *
        light_radiance * normal_dot_light;
}
