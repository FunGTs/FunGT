#if !defined(_MARCHING_CUBES_GPU_HPP_)
#define _MARCHING_CUBES_GPU_HPP_
#include "../marching_cubes.hpp"
#include <funlib/funlib.hpp>



    class MarchingCubesGPU : public MarchingCubes {

    sycl::queue m_queue;
    float *m_grid = nullptr;  

    public:
        MarchingCubesGPU(){

        }
        ~MarchingCubesGPU(){
            if (m_grid) {
                sycl::free(m_grid, m_queue);
                m_grid = nullptr;
            }
        }
        void createQueue(const std::string& name, flib::vendor v = flib::vendor::INTEL,
            flib::device dev = flib::device::GPU,
            flib::backend b = flib::backend::LEVEL_ZERO);
        template<typename FieldFunc>
        void createGridBuffer(FieldFunc field_function) {
            int nx = m_resolution[0];
            int ny = m_resolution[1];
            int nz = m_resolution[2];
            int total_grid_points = nx * ny * nz;
            float stepX = (m_boundsMax.x - m_boundsMin.x) / (float)(nx - 1);
            float stepY = (m_boundsMax.y - m_boundsMin.y) / (float)(ny - 1);
            float stepZ = (m_boundsMax.z - m_boundsMin.z) / (float)(nz - 1);

            int num_work_items1d = 16;
            int num_work_groups2d = num_work_items1d * num_work_items1d;

            int work_group2d_size = total_grid_points / num_work_groups2d;
            if (total_grid_points % num_work_groups2d != 0) {
                work_group2d_size++;
            }
            int work_group2d_size_x = static_cast<int>(std::sqrt(work_group2d_size));
            int work_group2d_size_y = work_group2d_size / work_group2d_size_x;

            if (work_group2d_size_x * work_group2d_size_y < work_group2d_size) {
                work_group2d_size_x++;
            }
            if (work_group2d_size_x * work_group2d_size_y < work_group2d_size) {
                work_group2d_size_y++;
            }

            sycl::range<2> global_range(work_group2d_size_x * num_work_items1d, work_group2d_size_y * num_work_items1d);
            sycl::range<2> local_range(num_work_items1d, num_work_items1d);

            float x0 = m_boundsMin.x;
            float y0 = m_boundsMin.y;
            float z0 = m_boundsMin.z;
            float hx = stepX;
            float hy = stepY;
            float hz = stepZ;
            float* grid_ptr = m_grid;

            m_queue.submit([=](sycl::handler& cgh) {
                cgh.parallel_for(sycl::nd_range<2>(global_range, local_range), [=](sycl::nd_item<2> item) {
                    int work_item_indx = item.get_local_id(1) * item.get_local_range(0) + item.get_local_id(0);
                    int work_group_indx = item.get_group(1) * item.get_group_range(0) + item.get_group(0);
                    int index = (item.get_local_range(0) * item.get_local_range(1)) * work_group_indx + work_item_indx;

                    if (index >= total_grid_points) return;

                    int idx = (index / nz) / ny;
                    int idy = (index / nz) % ny;
                    int idz = index % nz;

                    float xp = x0 + idx * hx;
                    float yp = y0 + idy * hy;
                    float zp = z0 + idz * hz;

                    grid_ptr[index] = field_function(xp, yp, zp);
                });
            }).wait();
        }
        void setField(std::function<float(fungt::Vec3)> field) override;
        void sampleScalarFieldInside(fungt::Vec3 min, fungt::Vec3 max) override;
        void setResolution(int x, int y, int z) override;
        void setIsovalue(float iso) override;

        void execute() override;

        const std::vector<PrimitiveVertex>& getVertices() const override;
        const std::vector<unsigned int>& getIndices() const override;


    };











#endif // _MARCHING_CUBES_GPU_HPP_
