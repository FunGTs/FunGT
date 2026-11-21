#if !defined(_MARCHING_CUBES_GPU_HPP_)
#define _MARCHING_CUBES_GPU_HPP_
#include "../marching_cubes.hpp"
#include <funlib/funlib.hpp>
#include <stdexcept>
#include "../mc_tables.hpp"
#include <CL/cl_gl.h>



    class MarchingCubesGPU : public fungt::MarchingCubes {

    sycl::queue m_queue;
    float *m_grid = nullptr;
    uint8_t*  m_devCubeIndex = nullptr;
    uint32_t* m_devTriCount = nullptr;
    uint32_t* m_devTriOffset = nullptr;
    int m_numCells = 0;
    std::string m_queue_name;
    int m_total_triangles = 0;
    int m_total_vertices = 0;     
    std::function<void(MarchingCubesGPU&)> m_gridSampler;
    std::function<void(MarchingCubesGPU&, unsigned int)> m_triangleGenerator;

    public:
        MarchingCubesGPU(){
            m_resolution = {64, 64, 64};
            m_isovalue = 0.0f;
        }
        ~MarchingCubesGPU(){
            if (m_grid) {
                sycl::free(m_grid, m_queue);
                m_grid = nullptr;
            }
            if (m_devCubeIndex) {
                sycl::free(m_devCubeIndex, m_queue);
                m_devCubeIndex = nullptr;
            }
            if (m_devTriCount) {
                sycl::free(m_devTriCount, m_queue);
                m_devTriCount = nullptr;
            }
            if (m_devTriOffset) {
                sycl::free(m_devTriOffset, m_queue);
                m_devTriOffset = nullptr;
            }
        }
        void initOGLinterop();
        void generateTriangles(unsigned int vboId);
        // Host callback retains resources needed by the device color callable.
        void setTriangleGenerator(std::function<void(MarchingCubesGPU&, unsigned int)> generator) {
            m_triangleGenerator = generator;
        }
        template<typename ColorFunc>
        void generateTriangles(unsigned int vboId, ColorFunc color_function)
        {
            int nx = m_resolution[0];
            int ny = m_resolution[1];
            int nz = m_resolution[2];
            int num_cells = m_numCells;
            int cx = nx - 1;
            int cy = ny - 1;

            float stepX = (m_boundsMax.x - m_boundsMin.x) / (float)(nx - 1);
            float stepY = (m_boundsMax.y - m_boundsMin.y) / (float)(ny - 1);
            float stepZ = (m_boundsMax.z - m_boundsMin.z) / (float)(nz - 1);
            float x0 = m_boundsMin.x;
            float y0 = m_boundsMin.y;
            float z0 = m_boundsMin.z;
            float isovalue = m_isovalue;

            uint8_t* d_cubeIndex = m_devCubeIndex;
            uint32_t* d_triOffset = m_devTriOffset;
            float* d_grid = m_grid;

            std::size_t n = static_cast<std::size_t>(num_cells);
            std::size_t xdim = static_cast<std::size_t>(std::ceil(std::sqrt(n)));
            std::size_t ydim = xdim;

            //OpenGL interop
            cl_context clcontext = flib::sycl_handler::get_clContext();

            cl_command_queue clqueue = sycl::get_native<sycl::backend::opencl>(m_queue);

            cl_mem clbuffer = clCreateFromGLBuffer(clcontext, CL_MEM_READ_WRITE, vboId, NULL);
            if (clbuffer == NULL) {
                throw std::runtime_error("Failed to create OpenCL buffer from GL buffer");
            }
            sycl::context syclCtx = flib::sycl_handler::get_sycl_context();
            // CRITICAL: Finish all pending OpenGL operations first
            glFinish();
            cl_event acquire_event;
            clEnqueueAcquireGLObjects(clqueue, 1, &clbuffer, 0, NULL, &acquire_event);
            clWaitForEvents(1, &acquire_event);  // Wait for acquisition to complete 
            {
                sycl::buffer<fungt::MCVertex> buf =
                    sycl::make_buffer<sycl::backend::opencl, fungt::MCVertex>(clbuffer, syclCtx);
            m_queue.submit([&](sycl::handler& cgh) {
                auto acc = buf.template get_access<sycl::access::mode::read_write>(cgh);
                cgh.parallel_for(sycl::range<2>(sycl::range<2>{static_cast<size_t>(xdim), static_cast<size_t>(ydim)}),
                    [=](sycl::item<2> item) {
                        std::size_t cellId = item[0] * ydim + item[1];
                        if (cellId >= n) return;

                        uint8_t cubeIdx = d_cubeIndex[cellId];
                        if (cubeIdx == 0 || cubeIdx == 255) return;

                        uint32_t writePos = d_triOffset[cellId] * 3;

                        int ix = cellId % cx;
                        int iy = (cellId / cx) % cy;
                        int iz = cellId / (cx * cy);

                        float vals[8];
                        vals[0] = d_grid[ix     + nx * (iy     + ny * iz)];
                        vals[1] = d_grid[(ix+1) + nx * (iy     + ny * iz)];
                        vals[2] = d_grid[(ix+1) + nx * (iy     + ny * (iz+1))];
                        vals[3] = d_grid[ix     + nx * (iy     + ny * (iz+1))];
                        vals[4] = d_grid[ix     + nx * ((iy+1) + ny * iz)];
                        vals[5] = d_grid[(ix+1) + nx * ((iy+1) + ny * iz)];
                        vals[6] = d_grid[(ix+1) + nx * ((iy+1) + ny * (iz+1))];
                        vals[7] = d_grid[ix     + nx * ((iy+1) + ny * (iz+1))];

                        float px = x0 + ix * stepX;
                        float py = y0 + iy * stepY;
                        float pz = z0 + iz * stepZ;

                        float corners[8][3] = {
                            {px,         py,         pz},
                            {px + stepX, py,         pz},
                            {px + stepX, py,         pz + stepZ},
                            {px,         py,         pz + stepZ},
                            {px,         py + stepY, pz},
                            {px + stepX, py + stepY, pz},
                            {px + stepX, py + stepY, pz + stepZ},
                            {px,         py + stepY, pz + stepZ}
                        };

                        constexpr int edgeConn[12][2] = {
                            {0,1}, {1,2}, {2,3}, {3,0},
                            {4,5}, {5,6}, {6,7}, {7,4},
                            {0,4}, {1,5}, {2,6}, {3,7}
                        };

                        float edgeVerts[12][3];
                        int edgeBits = fungt::edgeTable[cubeIdx];
                        for (int e = 0; e < 12; ++e) {
                            if (edgeBits & (1 << e)) {
                                int v0 = edgeConn[e][0];
                                int v1 = edgeConn[e][1];
                                float t = (isovalue - vals[v0]) / (vals[v1] - vals[v0]);
                                edgeVerts[e][0] = corners[v0][0] + t * (corners[v1][0] - corners[v0][0]);
                                edgeVerts[e][1] = corners[v0][1] + t * (corners[v1][1] - corners[v0][1]);
                                edgeVerts[e][2] = corners[v0][2] + t * (corners[v1][2] - corners[v0][2]);
                            }
                        }

                        for (int i = 0; fungt::triTable[cubeIdx][i] != -1; i += 3) {
                            int e0 = fungt::triTable[cubeIdx][i];
                            int e1 = fungt::triTable[cubeIdx][i + 1];
                            int e2 = fungt::triTable[cubeIdx][i + 2];

                            float v0x = edgeVerts[e0][0], v0y = edgeVerts[e0][1], v0z = edgeVerts[e0][2];
                            float v1x = edgeVerts[e1][0], v1y = edgeVerts[e1][1], v1z = edgeVerts[e1][2];
                            float v2x = edgeVerts[e2][0], v2y = edgeVerts[e2][1], v2z = edgeVerts[e2][2];

                            float ux = v1x - v0x, uy = v1y - v0y, uz = v1z - v0z;
                            float wx = v2x - v0x, wy = v2y - v0y, wz = v2z - v0z;
                            float nnx = uy * wz - uz * wy;
                            float nny = uz * wx - ux * wz;
                            float nnz = ux * wy - uy * wx;
                            float len = sycl::sqrt(nnx*nnx + nny*nny + nnz*nnz);
                            if (len > 0.0f) { nnx /= len; nny /= len; nnz /= len; }

                            acc[writePos].position = glm::vec3(v0x, v0y, v0z);
                            acc[writePos].normal = glm::vec3(nnx, nny, nnz);
                            acc[writePos].color = color_function(v0x, v0y, v0z);
                            writePos++;

                            acc[writePos].position = glm::vec3(v1x, v1y, v1z);
                            acc[writePos].normal = glm::vec3(nnx, nny, nnz);
                            acc[writePos].color = color_function(v1x, v1y, v1z);
                            writePos++;

                            acc[writePos].position = glm::vec3(v2x, v2y, v2z);
                            acc[writePos].normal = glm::vec3(nnx, nny, nnz);
                            acc[writePos].color = color_function(v2x, v2y, v2z);
                            writePos++;
                        }
                });

            }).wait();
            }
            clFinish(clqueue);
            //Release OpenCL buffer
            cl_event release_event;
            clEnqueueReleaseGLObjects(clqueue, 1, &clbuffer, 0, NULL, &release_event);
            clWaitForEvents(1, &release_event);  // Wait for release to complete

            glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
            clReleaseMemObject(clbuffer);


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

            m_grid = sycl::malloc_device<float>(total_grid_points, m_queue);

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

                    int idx = index % nx;
                    int idy = (index / nx) % ny;
                    int idz = (index / nx) / ny;

                    float xp = x0 + idx * hx;
                    float yp = y0 + idy * hy;
                    float zp = z0 + idz * hz;

                    grid_ptr[index] = field_function(xp, yp, zp);
                });
            }).wait();
        }
        void setField(std::function<float(fungt::Vec3)> field) override;
        void setGridSampler(std::function<void(MarchingCubesGPU&)> sampler) override {
            m_gridSampler = sampler;
        }
        sycl::queue& getQueue() { return m_queue; }
        void sampleScalarFieldInside(fungt::Vec3 min, fungt::Vec3 max) override;
        void setResolution(int x, int y, int z) override;
        void setIsovalue(float iso) override;

        void execute(unsigned int vbo_interop) override;
        
        const std::vector<fungt::MCVertex>& getVertices() const override;
        const std::vector<unsigned int>& getIndices() const override;

        template<typename FieldFunc>
        void prepare(FieldFunc field_function) {
            createQueue("mc_gpu");
            createGridBuffer(field_function);
            prepare();
        }
        void prepare() override;
        void classifyCells();
        void blelloch_scan();

        std::vector<uint32_t> getTriCountCPU();
        std::vector<uint32_t> getTriOffsetCPU();
        int getTotTriangles() const override {
            return m_total_triangles;
        }
        int getTotVertices() const override {
            return m_total_vertices;
        }
    };











#endif // _MARCHING_CUBES_GPU_HPP_
