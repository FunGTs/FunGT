inline float3 fgt_path_trace_cook_torrance(
    fgt_ray ray,
    __global const fgt_triangle_geom* triangle_geometry,
    __global const fgt_triangle_shading* triangle_shading,
    __global const fgt_material_data* materials,
    __global const fgt_bvh_node* bvh_nodes,
    __global const fgt_light* lights,
    __global const fgt_int32* emissive_triangles,
    __global const ulong* texture_handles,
    __global const int2* texture_dimensions,
    int num_triangles,
    int num_materials,
    int num_bvh_nodes,
    int num_lights,
    int num_emissive_triangles,
    int num_textures,
    __private fgt_rng* rng)
{
    float3 throughput = (float3)(1.0f);
    float3 radiance = (float3)(0.0f);

    for (int bounce = 0; bounce < 6; ++bounce) {
        fgt_geometry_hit geometry_hit;
        if (!fgt_trace_closest_bvh(
                ray,
                triangle_geometry,
                bvh_nodes,
                num_bvh_nodes,
                &geometry_hit)) {
            radiance += throughput * (float3)(0.3f);
            break;
        }

        fgt_surface_hit hit;
        if (!fgt_resolve_surface_hit(
                geometry_hit,
                triangle_shading,
                materials,
                num_triangles,
                num_materials,
                &hit)) {
            break;
        }

        const int texture_index =
            hit.material.base_color_texture_index;
        if (texture_index >= 0 && texture_index < num_textures) {
            const float3 texture_color = fgt_sample_bindless_texture(
                texture_handles,
                texture_dimensions,
                texture_index,
                hit.uv);
            hit.material.base_color[0] = texture_color.x;
            hit.material.base_color[1] = texture_color.y;
            hit.material.base_color[2] = texture_color.z;
        }

        const float3 normal = normalize(hit.shading_normal);
        const float3 view_direction = normalize(-ray.direction);
        float3 base_color;
        float metallic;
        float roughness;
        float3 f0;
        fgt_material_to_pbr(
            hit.material,
            &base_color,
            &metallic,
            &roughness,
            &f0);

        if (hit.material.emission > 0.0f) {
            radiance +=
                throughput * base_color * hit.material.emission;
        }

        float3 direct_light = (float3)(0.0f);
        for (int light_index = 0;
             light_index < num_lights;
             ++light_index) {
            const fgt_light light = lights[light_index];
            const float3 light_position = fgt_storage_vec3_to_float3(
                light.position);
            const float3 light_intensity = fgt_storage_vec3_to_float3(
                light.intensity);
            const float3 to_light = light_position - hit.point;
            const float distance_squared = dot(to_light, to_light);
            if (distance_squared <= 1.0e-12f) {
                continue;
            }

            const float distance = sqrt(distance_squared);
            const float3 light_direction = to_light / distance;
            fgt_ray shadow_ray;
            shadow_ray.origin =
                hit.point + hit.geometric_normal * 0.001f;
            shadow_ray.direction = light_direction;

            if (fgt_trace_shadow_bvh(
                    shadow_ray,
                    triangle_geometry,
                    bvh_nodes,
                    num_bvh_nodes,
                    distance - 0.001f)) {
                continue;
            }

            const float3 light_radiance =
                light_intensity / (distance_squared + 1.0e-6f);
            direct_light += fgt_evaluate_cook_torrance(
                normal,
                view_direction,
                light_direction,
                hit.material,
                light_radiance);
        }
        radiance += throughput * direct_light;

        // Sample emissive triangles only on early bounces.
        if (num_emissive_triangles > 0 && bounce < 3) {
            float3 light_position;
            float3 light_normal;
            float3 light_emission;
            float light_pdf;
            if (fgt_sample_emissive_triangle(
                    triangle_geometry,
                    triangle_shading,
                    materials,
                    emissive_triangles,
                    num_triangles,
                    num_materials,
                    num_emissive_triangles,
                    rng,
                    &light_position,
                    &light_normal,
                    &light_emission,
                    &light_pdf)) {
                const float3 to_light = light_position - hit.point;
                const float distance_squared = dot(to_light, to_light);
                if (distance_squared > 1.0e-12f) {
                    const float distance = sqrt(distance_squared);
                    const float3 light_direction = to_light / distance;
                    const float surface_cosine =
                        dot(normal, light_direction);
                    const float light_cosine =
                        dot(light_normal, -light_direction);

                    if (surface_cosine > 0.0f && light_cosine > 0.0f) {
                        fgt_ray shadow_ray;
                        shadow_ray.origin =
                            hit.point + hit.geometric_normal * 0.001f;
                        shadow_ray.direction = light_direction;
                        const bool blocked = fgt_trace_shadow_bvh(
                            shadow_ray,
                            triangle_geometry,
                            bvh_nodes,
                            num_bvh_nodes,
                            distance - 0.001f);

                        if (!blocked) {
                            const float3 nee =
                                fgt_evaluate_cook_torrance(
                                    normal,
                                    view_direction,
                                    light_direction,
                                    hit.material,
                                    light_emission);
                            const float geometry_term =
                                light_cosine / distance_squared;
                            radiance += throughput * nee *
                                geometry_term / light_pdf;
                        }
                    }
                }
            }
        }

        const float3 new_direction =
            fgt_sample_hemisphere(normal, rng);
        const float3 average_fresnel = fgt_fresnel_schlick(
            f0,
            fmax(dot(view_direction, normal), 0.0f));
        const float3 diffuse_weight =
            ((float3)(1.0f) - average_fresnel) * (1.0f - metallic);
        throughput *= diffuse_weight * base_color;

        ray.origin = hit.point + normal * 0.001f;
        ray.direction = new_direction;

        if (bounce > 2) {
            const float probability = fmin(
                0.95f,
                fmax(throughput.x,
                    fmax(throughput.y, throughput.z)));
            if (probability <= 0.0f ||
                fgt_rng_next_float(rng) > probability) {
                break;
            }
            throughput /= probability;
        }
    }

    return radiance;
}

__kernel void path_tracer(
    __global fgt_vec4* framebuffer,
    __global const fgt_triangle_geom* triangle_geometry,
    __global const fgt_triangle_shading* triangle_shading,
    __global const fgt_material_data* materials,
    __global const fgt_bvh_node* bvh_nodes,
    __global const fgt_light* lights,
    __global const fgt_int32* emissive_triangles,
    __global const ulong* texture_handles,
    __global const int2* texture_dimensions,
    int num_triangles,
    int num_materials,
    int num_bvh_nodes,
    int num_lights,
    int num_emissive_triangles,
    int num_textures,
    __constant const fgt_rayspace_camera* camera,
    int width,
    int height,
    int sample_index)
{
    const int x = (int)get_global_id(0);
    const int y = (int)get_global_id(1);

    if (x >= width || y >= height) {
        return;
    }

    const int pixel_index = y * width + x;
    const ulong seed =
        (ulong)pixel_index * 1337UL +
        (ulong)sample_index * 7919UL +
        123UL;
    fgt_rng rng = fgt_rng_create(seed, 1UL);

    const float width_denominator = (float)max(width - 1, 1);
    const float height_denominator = (float)max(height - 1, 1);
    const float image_u =
        ((float)x + fgt_rng_next_float(&rng)) / width_denominator;
    const float image_v =
        ((float)y + fgt_rng_next_float(&rng)) / height_denominator;
    const fgt_ray ray = fgt_camera_get_ray(camera, image_u, image_v);

    const float3 contribution = fgt_path_trace_cook_torrance(
        ray,
        triangle_geometry,
        triangle_shading,
        materials,
        bvh_nodes,
        lights,
        emissive_triangles,
        texture_handles,
        texture_dimensions,
        num_triangles,
        num_materials,
        num_bvh_nodes,
        num_lights,
        num_emissive_triangles,
        num_textures,
        &rng);

    fgt_vec4 accumulated = framebuffer[pixel_index];
    accumulated.x += contribution.x;
    accumulated.y += contribution.y;
    accumulated.z += contribution.z;
    accumulated.w = 1.0f;
    framebuffer[pixel_index] = accumulated;
}
