#if !defined(_MARCHING_CUBES_CPU_HPP_)
#define _MARCHING_CUBES_CPU_HPP_

#include "marching_cubes.hpp"

namespace fungt {

    class MarchingCubesCPU : public MarchingCubes {

    public:
        MarchingCubesCPU();
        ~MarchingCubesCPU();

        void setField(std::function<float(fungt::Vec3)> field) override;
        void sampleScalarFieldInside(fungt::Vec3 min, fungt::Vec3 max) override;
        void setResolution(int x, int y, int z) override;
        void setIsovalue(float iso) override;

        void execute() override;

        const std::vector<PrimitiveVertex>& getVertices() const override;
        const std::vector<unsigned int>& getIndices() const override;
    };

}

#endif // _MARCHING_CUBES_CPU_HPP_