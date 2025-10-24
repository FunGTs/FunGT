#if !defined(_DEFAULT_GEOMETRIES_H_)
#define _DEFAULT_GEOMETRIES_H_
#include "../../Triangle/triangle.hpp"
#include "../../Vector/vector3.hpp"
#include "../../Material/material.hpp"
#include <vector>


inline std::vector<Triangle> create_unit_cube(const Material& mat) {

    std::vector<Triangle> triangle_list; 

    // 8 corners of a unit cube centered at origin
    fungt::Vec3 p[8] = {
        {-2.5f, -2.5f, -2.5f}, // 0
        { 2.5f, -2.5f, -2.5f}, // 1
        { 2.5f,  2.5f, -2.5f}, // 2
        {-2.5f,  2.5f, -2.5f}, // 3
        {-2.5f, -2.5f,  2.5f}, // 4
        { 2.5f, -2.5f,  2.5f}, // 5
        { 2.5f,  2.5f,  2.5f}, // 6
        {-2.5f,  2.5f,  2.5f}  // 7
    };

    auto addFace = [&](int a, int b, int c, int d) {
        fungt::Vec3 normal = (p[b] - p[a]).cross(p[c] - p[a]).normalize();

        Triangle t1, t2;
        t1.v0 = p[a]; t1.v1 = p[b]; t1.v2 = p[c];
        t2.v0 = p[a]; t2.v1 = p[c]; t2.v2 = p[d];
        t1.normal = t2.normal = normal;
        t1.material = t2.material = mat;

        triangle_list.push_back(t1);
        triangle_list.push_back(t2);
    };

    // Faces (indices)
    addFace(0, 1, 2, 3); // -Z
    addFace(4, 5, 6, 7); // +Z
    addFace(0, 4, 7, 3); // -X
    addFace(1, 5, 6, 2); // +X
    addFace(3, 2, 6, 7); // +Y
    addFace(0, 1, 5, 4); // -Y

    return triangle_list;

}



#endif // _DEFAULT_GEOMETRIES_H_
