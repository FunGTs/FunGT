#if !defined(_MARCHING_CUBES_HPP_)
#define _MARCHING_CUBES_HPP_
#include <functional>
#include <vector>
#include <array>
#include <algorithm>
#include "Geometries/primitives.hpp"
#include "Vector/vector3.hpp"

class MarchingCubesGPU;

namespace fungt{



    enum class MC_MODE{
        CPU = 0,
        GPU = 1,
    };
    struct MCVertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;

        FUNGT_VERTEX_FORMAT(MCVertex, position, normal, color)
    };
    class MarchingCubes {

    protected:
        std::function<float(fungt::Vec3)> m_field;
        fungt::Vec3 m_boundsMin;
        fungt::Vec3 m_boundsMax;
        std::array<int, 3> m_resolution;
        float m_isovalue;

        std::vector<MCVertex> m_vertices;
        std::vector<unsigned int> m_indices;
    public:
        MarchingCubes() = default;
        virtual ~MarchingCubes() {

        }

        virtual void setField(std::function<float(fungt::Vec3)> field) {

        }
        // Host callback: prepares device data and submits grid sampling.
        virtual void setGridSampler(std::function<void(MarchingCubesGPU&)> sampler) {}
        virtual void sampleScalarFieldInside(fungt::Vec3 min, fungt::Vec3 max) {

        }
        virtual void setResolution(int x, int y, int z) {

        }
        virtual void setIsovalue(float iso) {

        }

        virtual void prepare() {

        }
        virtual void execute() {

        }
        virtual void execute(unsigned int vbo_interop){

        }
        virtual const std::vector<MCVertex>& getVertices() const {
            return m_vertices;
        }
        virtual const std::vector<unsigned int>& getIndices() const {
            return m_indices;
        }

        virtual int getTotTriangles() const {
            return static_cast<int>(m_indices.size() / 3);
        }

        virtual int getTotVertices() const {
            return static_cast<int>(m_vertices.size());
        }

    };



}


#endif // _IMARCHING_CUBES_HPP
