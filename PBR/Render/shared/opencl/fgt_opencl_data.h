#ifndef FGT_OPENCL_DATA_H
#define FGT_OPENCL_DATA_H

/*
 * Shared scene-transfer ABI for the C++ OpenCL host and OpenCL C kernels.
 * Keep this header free of C++ classes, constructors, namespaces and methods.
 */
#if defined(__OPENCL_C_VERSION__) || defined(__OPENCL_VERSION__)
typedef int fgt_int32;
#define FGT_DATA_ALIGN_16 __attribute__((aligned(16)))
#else
#include <cstddef>
#include <cstdint>
#include <type_traits>
typedef std::int32_t fgt_int32;
#define FGT_DATA_ALIGN_16 alignas(16)
#endif

typedef struct fgt_vec2 {
    float x;
    float y;
} fgt_vec2;

/* Compact storage type. Do not replace this with OpenCL float3. */
typedef struct fgt_vec3 {
    float x;
    float y;
    float z;
} fgt_vec3;

/* Explicitly aligned type for vector-friendly geometry loads. */
typedef struct FGT_DATA_ALIGN_16 fgt_vec4 {
    float x;
    float y;
    float z;
    float w;
} fgt_vec4;

typedef struct FGT_DATA_ALIGN_16 fgt_material_data {
    float base_color[3];
    float metallic;

    float roughness;
    float reflectance;
    float emission;
    fgt_int32 base_color_texture_index;
} fgt_material_data;

/* Hot intersection data: v0 plus precomputed triangle edges. */
typedef struct FGT_DATA_ALIGN_16 fgt_triangle_geom {
    fgt_vec4 v0;
    fgt_vec4 edge1;
    fgt_vec4 edge2;
} fgt_triangle_geom;

/* Cold data, fetched only after an intersection is accepted. */
typedef struct FGT_DATA_ALIGN_16 fgt_triangle_shading {
    fgt_vec3 n0;
    fgt_vec3 n1;
    fgt_vec3 n2;

    fgt_vec2 uv0;
    fgt_vec2 uv1;
    fgt_vec2 uv2;

    fgt_int32 material_index;
} fgt_triangle_shading;

typedef struct FGT_DATA_ALIGN_16 fgt_aabb {
    fgt_vec4 bounds_min;
    fgt_vec4 bounds_max;
} fgt_aabb;

typedef struct FGT_DATA_ALIGN_16 fgt_bvh_node {
    fgt_aabb bounds;
    fgt_int32 left_child;
    fgt_int32 right_child;
    fgt_int32 first_triangle_index;
    fgt_int32 triangle_count;
} fgt_bvh_node;

#define FGT_LIGHT_POINT 0
#define FGT_LIGHT_DIRECTIONAL 1
#define FGT_LIGHT_AREA 2

typedef struct FGT_DATA_ALIGN_16 fgt_light {
    fgt_vec3 position;
    fgt_int32 type;

    fgt_vec3 intensity;
    fgt_int32 padding;
} fgt_light;

/* Camera fields use 16-byte rows to keep the by-value/buffer ABI unambiguous. */
typedef struct FGT_DATA_ALIGN_16 fgt_rayspace_camera {
    fgt_vec4 origin_lens_radius;
    fgt_vec4 lower_left_corner;
    fgt_vec4 horizontal;
    fgt_vec4 vertical;
    fgt_vec4 basis_u;
    fgt_vec4 basis_v;
    fgt_vec4 basis_w;
} fgt_rayspace_camera;

#if !defined(__OPENCL_C_VERSION__) && !defined(__OPENCL_VERSION__)
static_assert(sizeof(fgt_int32) == 4);

static_assert(sizeof(fgt_vec2) == 8);
static_assert(sizeof(fgt_vec3) == 12);
static_assert(sizeof(fgt_vec4) == 16);
static_assert(alignof(fgt_vec4) == 16);

static_assert(sizeof(fgt_material_data) == 32);
static_assert(alignof(fgt_material_data) == 16);
static_assert(offsetof(fgt_material_data, metallic) == 12);
static_assert(offsetof(fgt_material_data, roughness) == 16);
static_assert(offsetof(fgt_material_data, base_color_texture_index) == 28);

static_assert(sizeof(fgt_triangle_geom) == 48);
static_assert(alignof(fgt_triangle_geom) == 16);
static_assert(sizeof(fgt_triangle_shading) == 64);
static_assert(alignof(fgt_triangle_shading) == 16);
static_assert(offsetof(fgt_triangle_shading, material_index) == 60);

static_assert(sizeof(fgt_aabb) == 32);
static_assert(sizeof(fgt_bvh_node) == 48);
static_assert(sizeof(fgt_light) == 32);
static_assert(sizeof(fgt_rayspace_camera) == 112);

static_assert(std::is_standard_layout_v<fgt_material_data>);
static_assert(std::is_trivially_copyable_v<fgt_material_data>);
static_assert(std::is_standard_layout_v<fgt_triangle_geom>);
static_assert(std::is_trivially_copyable_v<fgt_triangle_geom>);
static_assert(std::is_standard_layout_v<fgt_triangle_shading>);
static_assert(std::is_trivially_copyable_v<fgt_triangle_shading>);
#endif

#undef FGT_DATA_ALIGN_16

#endif
