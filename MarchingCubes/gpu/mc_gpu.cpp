#include "mc_gpu.hpp"
#include "../mc_tables.hpp"



void MarchingCubesGPU::initOGLinterop()
{
}

void MarchingCubesGPU::generateTriangles(unsigned int vboId)
{
    generateTriangles(vboId, [](float, float, float) { return glm::vec3(0.8f); });
}

void MarchingCubesGPU::createQueue(const std::string& name, flib::vendor v, flib::device dev, flib::backend b)
{
    try {
        m_queue_name = name;
        flib::sycl_handler::register_queue(m_queue_name, flib::device::GPU, flib::vendor::INTEL, flib::backend::OPENCL);
        flib::sycl_handler::create_gl_interop_context(m_queue_name);
        flib::sycl_handler::get_device_info(m_queue_name);
   
        m_queue = flib::sycl_handler::get_queue(name);

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
void MarchingCubesGPU::setField(std::function<float(fungt::Vec3)> field)
{
    m_field = field;
}

void MarchingCubesGPU::sampleScalarFieldInside(fungt::Vec3 min, fungt::Vec3 max)
{
    m_boundsMin = min;
    m_boundsMax = max;
}

void MarchingCubesGPU::setResolution(int x, int y, int z)
{
    m_resolution = { x, y, z };
}

void MarchingCubesGPU::setIsovalue(float iso)
{
    m_isovalue = iso;
}

void MarchingCubesGPU::prepare()
{
    if (m_gridSampler) {
        if (m_queue_name.empty()) createQueue("mc_gpu");
        if (m_grid) {
            sycl::free(m_grid, m_queue);
            m_grid = nullptr;
        }
        m_gridSampler(*this);
    }
    if (!m_grid) {
        throw std::logic_error("GPU preparation requires prepare(field_function) or createGridBuffer(field_function) first");
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
    classifyCells();
    blelloch_scan();
}

void MarchingCubesGPU::execute(unsigned int vbo_interop)
{
    if (m_triangleGenerator) {
        m_triangleGenerator(*this, vbo_interop);
    } else {
        generateTriangles(vbo_interop);
    }
}

const std::vector<fungt::MCVertex>& MarchingCubesGPU::getVertices() const
{
    return m_vertices;
}

const std::vector<unsigned int>& MarchingCubesGPU::getIndices() const
{
    return m_indices;
}


void MarchingCubesGPU::classifyCells()
{
    int nx = m_resolution[0];
    int ny = m_resolution[1];
    int nz = m_resolution[2];

    int num_cells = (nx - 1) * (ny - 1) * (nz - 1);
    m_numCells = num_cells;

    m_devCubeIndex = sycl::malloc_device<uint8_t>(num_cells, m_queue);
    m_devTriCount = sycl::malloc_device<uint32_t>(num_cells, m_queue);

    int num_work_items1d = 16;
    int num_work_groups2d = num_work_items1d * num_work_items1d;

    int work_group2d_size = num_cells / num_work_groups2d;
    if (num_cells % num_work_groups2d != 0) {
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

    float* grid_ptr = m_grid;
    float isovalue = m_isovalue;
    int cx = nx - 1;
    int cy = ny - 1;
    uint8_t* d_cubeIndex = m_devCubeIndex;
    uint32_t* d_triCount = m_devTriCount;

    m_queue.submit([=](sycl::handler& cgh) {
        cgh.parallel_for(sycl::nd_range<2>(global_range, local_range), [=](sycl::nd_item<2> item) {
            int work_item_indx = item.get_local_id(1) * item.get_local_range(0) + item.get_local_id(0);
            int work_group_indx = item.get_group(1) * item.get_group_range(0) + item.get_group(0);
            int cellId = (item.get_local_range(0) * item.get_local_range(1)) * work_group_indx + work_item_indx;

            if (cellId >= num_cells) return;

            int iz = (cellId / cx) / cy;
            int iy = (cellId / cx) % cy;
            int ix = cellId % cx;

            float v0 = grid_ptr[ix     + nx * (iy     + ny * iz)];
            float v1 = grid_ptr[(ix+1) + nx * (iy     + ny * iz)];
            float v2 = grid_ptr[(ix+1) + nx * (iy     + ny * (iz+1))];
            float v3 = grid_ptr[ix     + nx * (iy     + ny * (iz+1))];
            float v4 = grid_ptr[ix     + nx * ((iy+1) + ny * iz)];
            float v5 = grid_ptr[(ix+1) + nx * ((iy+1) + ny * iz)];
            float v6 = grid_ptr[(ix+1) + nx * ((iy+1) + ny * (iz+1))];
            float v7 = grid_ptr[ix     + nx * ((iy+1) + ny * (iz+1))];

            uint8_t cubeIdx = 0;
            if (v0 < isovalue) cubeIdx |= 1;
            if (v1 < isovalue) cubeIdx |= 2;
            if (v2 < isovalue) cubeIdx |= 4;
            if (v3 < isovalue) cubeIdx |= 8;
            if (v4 < isovalue) cubeIdx |= 16;
            if (v5 < isovalue) cubeIdx |= 32;
            if (v6 < isovalue) cubeIdx |= 64;
            if (v7 < isovalue) cubeIdx |= 128;

            uint32_t count = 0;
            for (int i = 0; fungt::triTable[cubeIdx][i] != -1; i += 3) {
                count++;
            }

            d_cubeIndex[cellId] = cubeIdx;
            d_triCount[cellId] = count;
        });
    }).wait();
}

void blelloch_scan_device(sycl::queue& queue, uint32_t* d_output, std::size_t n)
{
    if (n == 0) return;

    std::size_t work_group_size = 64; // desired work group size
    std::size_t n_work_groups;
    std::size_t global_size =
        ((n + work_group_size - 1) / work_group_size) * work_group_size;
    n_work_groups = global_size / work_group_size; // number of work groups

    uint32_t* d_block_sums = sycl::malloc_device<uint32_t>(n_work_groups, queue);

    // Blelloch scan per work group
    queue.submit([=](sycl::handler& h) {
        sycl::local_accessor<uint32_t, 1> local_sum(sycl::range<1>(work_group_size), h);

        h.parallel_for(sycl::nd_range<1>{global_size, work_group_size}, [=](sycl::nd_item<1> item) {
            std::size_t l_id = item.get_local_id(0);
            std::size_t wg_id = item.get_group(0);
            std::size_t wg_size = item.get_local_range(0);
            std::size_t index = wg_id * wg_size + l_id;

            // Load global to local
            if (index < n) {
                local_sum[l_id] = d_output[index];
            } else {
                local_sum[l_id] = 0;
            }

            // Up-sweep: total ends at last element
            for (std::size_t stride = 1; stride < wg_size; stride *= 2) {
                item.barrier(sycl::access::fence_space::local_space);
                std::size_t idx = (l_id + 1) * stride * 2 - 1;
                if (idx < wg_size) {
                    local_sum[idx] += local_sum[idx - stride];
                }
            }

            item.barrier(sycl::access::fence_space::local_space);

            // Save block total, clear last for exclusive scan
            if (l_id == 0) {
                d_block_sums[wg_id] = local_sum[wg_size - 1];
                local_sum[wg_size - 1] = 0;
            }

            // Down-sweep
            for (std::size_t stride = wg_size / 2; stride > 0; stride /= 2) {
                item.barrier(sycl::access::fence_space::local_space);
                std::size_t idx = (l_id + 1) * stride * 2 - 1;
                if (idx < wg_size) {
                    uint32_t t = local_sum[idx - stride];
                    local_sum[idx - stride] = local_sum[idx];
                    local_sum[idx] += t;
                }
            }

            item.barrier(sycl::access::fence_space::local_space);

            // Write back to global
            if (index < n) {
                d_output[index] = local_sum[l_id];
            }
        });
    }).wait();

    // A single block already contains the complete exclusive scan.
    if (n_work_groups == 1) {
        sycl::free(d_block_sums, queue);
        return;
    }

    // Scan every level of block sums before propagating offsets back down.
    blelloch_scan_device(queue, d_block_sums, n_work_groups);

    // Add the sum of all preceding blocks to each local offset
    queue.submit([=](sycl::handler& h) {
        h.parallel_for(sycl::nd_range<1>{global_size, work_group_size}, [=](sycl::nd_item<1> item) {
            std::size_t l_id = item.get_local_id(0);
            std::size_t wg_id = item.get_group(0);
            std::size_t index = wg_id * item.get_local_range(0) + l_id;

            if (index < n) {
                d_output[index] += d_block_sums[wg_id];
            }
        });
    }).wait();

    sycl::free(d_block_sums, queue);
}


void MarchingCubesGPU::blelloch_scan()
{
    int n = m_numCells;
    if (n == 0) return;

    m_devTriOffset = sycl::malloc_device<uint32_t>(n, m_queue);
    m_queue.memcpy(m_devTriOffset, m_devTriCount, n * sizeof(uint32_t)).wait();

    blelloch_scan_device(m_queue, m_devTriOffset, static_cast<std::size_t>(n));

    uint32_t lastOffset, lastCount;
    m_queue.memcpy(&lastOffset, m_devTriOffset + (n - 1), sizeof(uint32_t)).wait();
    m_queue.memcpy(&lastCount, m_devTriCount + (n - 1), sizeof(uint32_t)).wait();
    m_total_triangles = lastOffset + lastCount; 
    m_total_vertices  = m_total_triangles * 3; 

    std::cout << "Triangles: " << m_total_triangles << ", Vertices: " << m_total_vertices * 3 << std::endl;
}

std::vector<uint32_t> MarchingCubesGPU::getTriCountCPU()
{
    std::vector<uint32_t> result(m_numCells);
    m_queue.memcpy(result.data(), m_devTriCount, m_numCells * sizeof(uint32_t)).wait();
    return result;
}

std::vector<uint32_t> MarchingCubesGPU::getTriOffsetCPU()
{
    std::vector<uint32_t> result(m_numCells);
    m_queue.memcpy(result.data(), m_devTriOffset, m_numCells * sizeof(uint32_t)).wait();
    return result;
}
