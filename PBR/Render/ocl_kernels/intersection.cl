typedef struct fgt_geometry_hit {
    float distance;
    float3 point;
    float3 barycentric;
    float3 geometric_normal;
    int triangle_index;
} fgt_geometry_hit;

inline bool fgt_intersect_triangle(
    fgt_ray ray,
    __global const fgt_triangle_geom* triangle,
    float minimum_distance,
    float maximum_distance,
    __private fgt_geometry_hit* hit)
{
    const float epsilon = 1.0e-8f;
    const float3 vertex0 = fgt_vec4_xyz(triangle->v0);
    const float3 edge1 = fgt_vec4_xyz(triangle->edge1);
    const float3 edge2 = fgt_vec4_xyz(triangle->edge2);

    const float3 direction_cross_edge2 = cross(ray.direction, edge2);
    const float determinant = dot(edge1, direction_cross_edge2);
    if (fabs(determinant) < epsilon) {
        return false;
    }

    const float inverse_determinant = 1.0f / determinant;
    const float3 origin_from_vertex = ray.origin - vertex0;
    const float barycentric_u =
        inverse_determinant * dot(origin_from_vertex, direction_cross_edge2);
    if (barycentric_u < 0.0f || barycentric_u > 1.0f) {
        return false;
    }

    const float3 origin_cross_edge1 = cross(origin_from_vertex, edge1);
    const float barycentric_v =
        inverse_determinant * dot(ray.direction, origin_cross_edge1);
    if (barycentric_v < 0.0f ||
        barycentric_u + barycentric_v > 1.0f) {
        return false;
    }

    const float distance =
        inverse_determinant * dot(edge2, origin_cross_edge1);
    if (distance < minimum_distance || distance > maximum_distance) {
        return false;
    }

    hit->distance = distance;
    hit->point = ray.origin + ray.direction * distance;
    hit->barycentric = (float3)(
        1.0f - barycentric_u - barycentric_v,
        barycentric_u,
        barycentric_v);
    hit->geometric_normal = normalize(cross(edge1, edge2));
    return true;
}

inline bool fgt_intersect_aabb(
    fgt_ray ray,
    fgt_aabb bounds,
    float minimum_distance,
    float maximum_distance)
{
    const float3 bounds_min = fgt_vec4_xyz(bounds.bounds_min);
    const float3 bounds_max = fgt_vec4_xyz(bounds.bounds_max);

    for (int axis = 0; axis < 3; ++axis) {
        const float inverse_direction = 1.0f / ray.direction[axis];
        float near_distance =
            (bounds_min[axis] - ray.origin[axis]) * inverse_direction;
        float far_distance =
            (bounds_max[axis] - ray.origin[axis]) * inverse_direction;

        if (inverse_direction < 0.0f) {
            const float temporary = near_distance;
            near_distance = far_distance;
            far_distance = temporary;
        }

        minimum_distance = fmax(minimum_distance, near_distance);
        maximum_distance = fmin(maximum_distance, far_distance);
        if (maximum_distance < minimum_distance) {
            return false;
        }
    }

    return true;
}
