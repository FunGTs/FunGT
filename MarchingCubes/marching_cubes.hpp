#if !defined(_MARCHING_CUBES_HPP_)
#define _MARCHING_CUBES_HPP_

#include <functional>
#include <vector>
#include <array>
#include <algorithm>
#include "Geometries/primitives.hpp"
#include "Vector/vector3.hpp"

namespace fungt {

    class MarchingCubes {
    private:
        std::function<float(fungt::Vec3)> m_field;
        fungt::Vec3 m_boundsMin;
        fungt::Vec3 m_boundsMax;
        std::array<int, 3> m_resolution;
        float m_isovalue;

        std::vector<PrimitiveVertex> m_vertices;
        std::vector<unsigned int> m_indices;

    public:
        MarchingCubes();
        ~MarchingCubes();

        void setField(std::function<float(fungt::Vec3)> field);
        void sampleScalarFieldInside(fungt::Vec3 min, fungt::Vec3 max);
        void setResolution(int x, int y, int z);
        void setIsovalue(float iso);

        void execute();

        const std::vector<PrimitiveVertex>& getVertices() const;
        const std::vector<unsigned int>& getIndices() const;
    };

}

#endif // _MARCHING_CUBES_HPP_