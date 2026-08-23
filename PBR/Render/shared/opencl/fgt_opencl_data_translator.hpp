#ifndef FGT_OPENCL_DATA_TRANSLATOR_HPP
#define FGT_OPENCL_DATA_TRANSLATOR_HPP

#include "PBR/Render/shared/opencl/fgt_opencl_data.h"

#include "PBR/BVH/bvh_node.hpp"
#include "PBR/Light/light.hpp"
#include "PBR/PBRCamera/pbr_camera.hpp"
#include "Triangle/triangle.hpp"
#include "Vector/vector3.hpp"

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

namespace fgt::opencl {

struct fgt_opencl_scene_data {
    std::vector<fgt_triangle_geom> triangle_geometry;
    std::vector<fgt_triangle_shading> triangle_shading;
    std::vector<fgt_material_data> materials;
    std::vector<fgt_bvh_node> bvh_nodes;
};

inline fgt_vec3 translate_vec3(const fungt::Vec3& value)
{
    return {value.x, value.y, value.z};
}

inline fgt_vec4 translate_vec4(const fungt::Vec3& value, float w = 0.0f)
{
    return {value.x, value.y, value.z, w};
}

inline fgt_material_data translate_material(const MaterialData& material)
{
    return {
        {
            material.baseColor[0],
            material.baseColor[1],
            material.baseColor[2]
        },
        material.metallic,
        material.roughness,
        material.reflectance,
        material.emission,
        static_cast<fgt_int32>(material.baseColorTexIdx)
    };
}

inline bool materials_equal(
    const fgt_material_data& lhs,
    const fgt_material_data& rhs)
{
    return lhs.base_color[0] == rhs.base_color[0] &&
           lhs.base_color[1] == rhs.base_color[1] &&
           lhs.base_color[2] == rhs.base_color[2] &&
           lhs.metallic == rhs.metallic &&
           lhs.roughness == rhs.roughness &&
           lhs.reflectance == rhs.reflectance &&
           lhs.emission == rhs.emission &&
           lhs.base_color_texture_index == rhs.base_color_texture_index;
}

inline fgt_int32 find_or_add_material(
    const MaterialData& source,
    std::vector<fgt_material_data>& materials)
{
    const fgt_material_data translated = translate_material(source);

    for (std::size_t index = 0; index < materials.size(); ++index) {
        if (materials_equal(materials[index], translated)) {
            return static_cast<fgt_int32>(index);
        }
    }

    if (materials.size() >=
        static_cast<std::size_t>(std::numeric_limits<fgt_int32>::max())) {
        throw std::overflow_error(
            "OpenCL material table exceeds the signed 32-bit index range.");
    }

    materials.push_back(translated);
    return static_cast<fgt_int32>(materials.size() - 1);
}

inline fgt_triangle_geom translate_triangle_geometry(const Triangle& triangle)
{
    const fungt::Vec3 edge1 = triangle.v1 - triangle.v0;
    const fungt::Vec3 edge2 = triangle.v2 - triangle.v0;

    return {
        translate_vec4(triangle.v0),
        translate_vec4(edge1),
        translate_vec4(edge2)
    };
}

inline fgt_triangle_shading translate_triangle_shading(
    const Triangle& triangle,
    fgt_int32 material_index)
{
    return {
        translate_vec3(triangle.n0),
        translate_vec3(triangle.n1),
        translate_vec3(triangle.n2),
        {triangle.uvs[0][0], triangle.uvs[0][1]},
        {triangle.uvs[1][0], triangle.uvs[1][1]},
        {triangle.uvs[2][0], triangle.uvs[2][1]},
        material_index
    };
}

inline fgt_aabb translate_aabb(const AABB& bounds)
{
    return {
        translate_vec4(bounds.m_min),
        translate_vec4(bounds.m_max)
    };
}

inline fgt_bvh_node translate_bvh_node(const BVHNode& node)
{
    return {
        translate_aabb(node.m_boundingBox),
        static_cast<fgt_int32>(node.leftChild),
        static_cast<fgt_int32>(node.rightChild),
        static_cast<fgt_int32>(node.firstTriIdx),
        static_cast<fgt_int32>(node.triCount)
    };
}

inline fgt_int32 translate_light_type(LightType type)
{
    switch (type) {
        case LightType::Point:
            return FGT_LIGHT_POINT;
        case LightType::Directional:
            return FGT_LIGHT_DIRECTIONAL;
        case LightType::Area:
            return FGT_LIGHT_AREA;
    }

    throw std::invalid_argument("Unknown light type for OpenCL translation.");
}

inline fgt_light translate_light(const Light& light)
{
    return {
        translate_vec3(light.m_pos),
        translate_light_type(light.m_type),
        translate_vec3(light.m_intensity),
        0
    };
}

inline fgt_rayspace_camera translate_rayspace_camera(const PBRCamera& camera)
{
    return {
        translate_vec4(camera.getOrigin(), camera.getLensRadius()),
        translate_vec4(camera.getLowerLeftCorner()),
        translate_vec4(camera.getHorizontal()),
        translate_vec4(camera.getVertical()),
        translate_vec4(camera.getBasisU()),
        translate_vec4(camera.getBasisV()),
        translate_vec4(camera.getBasisW())
    };
}
inline fgt_opencl_scene_data translate_scene_data(
    const std::vector<Triangle>& triangles,
    const std::vector<BVHNode>& nodes)
{
    fgt_opencl_scene_data result;

    result.triangle_geometry.reserve(triangles.size());
    result.triangle_shading.reserve(triangles.size());

    for (const Triangle& triangle : triangles) {
        const fgt_int32 material_index =
            find_or_add_material(triangle.material, result.materials);

        result.triangle_geometry.push_back(
            translate_triangle_geometry(triangle));
        result.triangle_shading.push_back(
            translate_triangle_shading(triangle, material_index));
    }

    result.bvh_nodes.reserve(nodes.size());
    for (const BVHNode& node : nodes) {
        result.bvh_nodes.push_back(translate_bvh_node(node));
    }

    return result;
}
inline std::vector<fgt_light> load_scene_lights(const std::vector<Light>& lights) {

    std::vector<fgt_light> result;
    result.reserve(lights.size());
    for (const Light& light : lights) {
        result.push_back(translate_light(light));
    }

    return result;
}

} // namespace fgt::opencl

#endif
