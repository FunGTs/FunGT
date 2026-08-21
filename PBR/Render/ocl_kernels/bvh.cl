#define FGT_BVH_STACK_SIZE 32
#define FGT_FLOAT_MAX 3.402823466e+38f

inline bool fgt_trace_closest_bvh(
    fgt_ray ray,
    __global const fgt_triangle_geom* triangle_geometry,
    __global const fgt_bvh_node* bvh_nodes,
    int num_bvh_nodes,
    __private fgt_geometry_hit* closest_hit)
{
    if (num_bvh_nodes <= 0) {
        return false;
    }

    int stack[FGT_BVH_STACK_SIZE];
    int stack_size = 0;
    stack[stack_size++] = 0;

    bool found_hit = false;
    float closest_distance = FGT_FLOAT_MAX;

    while (stack_size > 0) {
        const int node_index = stack[--stack_size];
        if (node_index < 0 || node_index >= num_bvh_nodes) {
            continue;
        }

        const fgt_bvh_node node = bvh_nodes[node_index];
        if (!fgt_intersect_aabb(
                ray,
                node.bounds,
                0.001f,
                closest_distance)) {
            continue;
        }

        if (node.triangle_count > 0) {
            for (int offset = 0; offset < node.triangle_count; ++offset) {
                const int triangle_index =
                    node.first_triangle_index + offset;
                fgt_geometry_hit candidate;

                if (fgt_intersect_triangle(
                        ray,
                        &triangle_geometry[triangle_index],
                        0.001f,
                        closest_distance,
                        &candidate)) {
                    found_hit = true;
                    closest_distance = candidate.distance;
                    candidate.triangle_index = triangle_index;
                    *closest_hit = candidate;
                }
            }
        }
        else {
            stack[stack_size++] = node.left_child;
            stack[stack_size++] = node.right_child;
        }
    }

    return found_hit;
}

inline bool fgt_trace_shadow_bvh(
    fgt_ray ray,
    __global const fgt_triangle_geom* triangle_geometry,
    __global const fgt_bvh_node* bvh_nodes,
    int num_bvh_nodes,
    float maximum_distance)
{
    if (num_bvh_nodes <= 0) {
        return false;
    }

    int stack[FGT_BVH_STACK_SIZE];
    int stack_size = 0;
    stack[stack_size++] = 0;

    while (stack_size > 0) {
        const int node_index = stack[--stack_size];
        if (node_index < 0 || node_index >= num_bvh_nodes) {
            continue;
        }

        const fgt_bvh_node node = bvh_nodes[node_index];
        if (!fgt_intersect_aabb(
                ray,
                node.bounds,
                0.001f,
                maximum_distance)) {
            continue;
        }

        if (node.triangle_count > 0) {
            for (int offset = 0; offset < node.triangle_count; ++offset) {
                const int triangle_index =
                    node.first_triangle_index + offset;
                fgt_geometry_hit candidate;

                if (fgt_intersect_triangle(
                        ray,
                        &triangle_geometry[triangle_index],
                        0.001f,
                        maximum_distance,
                        &candidate)) {
                    return true;
                }
            }
        }
        else {
            stack[stack_size++] = node.left_child;
            stack[stack_size++] = node.right_child;
        }
    }

    return false;
}

#undef FGT_BVH_STACK_SIZE
#undef FGT_FLOAT_MAX
