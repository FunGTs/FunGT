#if !defined(_MARCHING_CUBES_HPP_)
#define _MARCHING_CUBES_HPP_
#include <functional>
#include <vector>
#include <array>
#include <algorithm>
#include "Geometries/primitives.hpp"
#include "Vector/vector3.hpp"

namespace fungt{




    class MarchingCubes {

    protected:
        std::function<float(fungt::Vec3)> m_field;
        fungt::Vec3 m_boundsMin;
        fungt::Vec3 m_boundsMax;
        std::array<int, 3> m_resolution;
        float m_isovalue;

        std::vector<PrimitiveVertex> m_vertices;
        std::vector<unsigned int> m_indices;
    public:
        MarchingCubes() = default;
        virtual ~MarchingCubes() {

        }

        virtual void setField(std::function<float(fungt::Vec3)> field) {

        }
        virtual void sampleScalarFieldInside(fungt::Vec3 min, fungt::Vec3 max) {

        }
        virtual void setResolution(int x, int y, int z) {

        }
        virtual void setIsovalue(float iso) {

        }

        virtual void execute() {

        }

        virtual const std::vector<PrimitiveVertex>& getVertices() const {
            return m_vertices;
        }
        virtual const std::vector<unsigned int>& getIndices() const {
            return m_indices;
        }

    };



}


#endif // _IMARCHING_CUBES_HPP
