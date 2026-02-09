#include "clothing.hpp"


Clothing::Clothing(int nx, int ny ){
    std::cout<<"Clothin constructor"<<std::endl;
    grid_x  = nx;
    grid_y  = ny;

    GridSize= size_t(grid_x) * size_t(grid_y);
    posIn.resize(GridSize);
    velIn.resize(GridSize); 
    velOut.resize(GridSize);

    RestLengthHoriz = 1.0f / float(grid_x - 1); // normalized spacing
    RestLengthVert  = 1.0f / float(grid_y - 1);
    RestLengthDiag  = std::sqrt(RestLengthHoriz*RestLengthHoriz + RestLengthVert*RestLengthVert);

        // Initialize cloth flat in X-Y plane, z = 0.0f
    for (int y = 0; y < grid_y; ++y) {
        for (int x = 0; x < grid_x; ++x) {
            size_t global_id = size_t(y) * grid_x + x;
            posIn[global_id].x = float(x) * RestLengthHoriz - 0.5f; // horizontal
            posIn[global_id].y = float(y) * RestLengthVert - 0.5f; // vertical
            posIn[global_id].z = 0.0f; // depth
            //printf("%f   %f   %f\n", posIn[global_id].x, posIn[global_id].y, posIn[global_id].z);
            velIn[global_id].x = velIn[global_id].y = velIn[global_id].z = 0.0f;
        }
    }
    std::string ps_vs = getAssetPath("resources/clothing.vs");
    std::string ps_fs = getAssetPath("resources/clothing.fs");
    initIndices();
    this->init();
    m_shader.create(ps_vs, ps_fs);
}

void Clothing::initIndices()
{
    // Calculate number of indices needed
    int numResetValues = grid_y - 1;
    numIndices = (grid_y - 1) * grid_x * 2 + numResetValues;
    indexData.resize(numIndices);
    std::cout<<"TOTAL INDICES : "<<numIndices<<std::endl;
    int index = 0;
    

    for (int row = 0; row < grid_y - 1; row++) {
        for (int col = 0; col < grid_x; col++) {
            indexData[index++] = row * grid_x + col;        // Current row
            indexData[index++] = (row + 1) * grid_x + col;  // Next row
        }
        // Primitive restart at end of each strip
        indexData[index++] = PRIM_RESTART;
    }
}

void Clothing::init()
{
    m_vao.genVAO();
    m_vao.bind();


    m_vbo[0].genVB();
    m_vbo[0].bind();
    //Send just the out positions to Opengl buffers
    m_vbo[0].bufferData(posIn.data(), posIn.size() * sizeof(fungt::Vec3), GL_DYNAMIC_DRAW);
    
    m_vbo[1].genVB();
    m_vbo[1].bind();
    //Send just the out positions to Opengl buffers
    m_vbo[1].bufferData(posIn.data(), posIn.size() * sizeof(fungt::Vec3), GL_DYNAMIC_DRAW);
   
    // Setup vertex attributes using buffer[0] initially
    m_vbo[0].bind();
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, sizeof(fungt::Vec3), (void *)0);
    glEnableVertexAttribArray(0);

     m_vi.genVI(); //Generates the Vertex Buffer
     m_vi.bind();
     m_vi.indexData(indexData.data(), indexData.size() * sizeof(indexData[0]));


    m_vao.unbind();
}

void Clothing::simulation()
{
    // Get the SYCL queue from the sycl_handler   
    sycl::queue Q = flib::sycl_handler::get_queue();

    
    //OpenCL interop for updating particles
    cl_context clcontext = flib::sycl_handler::get_clContext();
    cl_command_queue clqueue = sycl::get_native<sycl::backend::opencl>(Q);
    // Create OpenCL buffers from BOTH OpenGL buffers
    cl_mem cl_posInBuff = clCreateFromGLBuffer(clcontext, CL_MEM_READ_ONLY, m_vbo[currentBuffer].getId(), NULL);
    cl_mem cl_posOutBuff = clCreateFromGLBuffer(clcontext, CL_MEM_WRITE_ONLY, m_vbo[1 - currentBuffer].getId(), NULL);
    
    if (cl_posInBuff == NULL) {
        throw std::runtime_error("Failed to create l_posInBuff from GL buffer");
    }
    if (cl_posOutBuff == NULL) {
        throw std::runtime_error("Failed to create cl_posOutBuff from GL buffer");
    }
    sycl::context syclCtx = flib::sycl_handler::get_sycl_context();

    cl_mem cl_buffers_to_acquire[] = { cl_posInBuff, cl_posOutBuff };
    clEnqueueAcquireGLObjects(clqueue, 2, cl_buffers_to_acquire, 0, NULL, NULL);

    //std::cout<<"SUCESS : Cross OpenCL-OpenGL setup "<<std::endl;
    { //SYCL Scope
        // Copy member values to local PODs so kernel can use them
        const size_t gridWidth = static_cast<size_t>(grid_x);
        const size_t gridHeight = static_cast<size_t>(grid_y);
        const float gravityY = GravityY;
        const float k = SpringK;
        const float dt = DeltaT;
        const float invM = ParticleInvMass;
        const float damp = DampingConst;
        const float restH = RestLengthHoriz;
        const float restV = RestLengthVert;
        const float restD = RestLengthDiag;
        const float particleMass = ParticleMass;
        //Buffers
    
        sycl::buffer<fungt::Vec3, 1> velInBuff(velIn.data(), sycl::range<1>(GridSize));
        sycl::buffer<fungt::Vec3, 1> velOutBuff(velOut.data(), sycl::range<1>(GridSize));

        sycl::buffer<fungt::Vec3> inPosBuff = sycl::make_buffer<sycl::backend::opencl, fungt::Vec3>(cl_posInBuff, syclCtx);

        sycl::buffer<fungt::Vec3> outPosBuff = sycl::make_buffer<sycl::backend::opencl, fungt::Vec3>(cl_posOutBuff, syclCtx);
        // SYCL event to track kernel completion
        sycl::event kernel_completion_event;
        kernel_completion_event = Q.submit([&](sycl::handler& h) {
            auto posInAcc = inPosBuff.get_access<sycl::access::mode::read>(h);
            auto posOutAcc = outPosBuff.get_access<sycl::access::mode::write>(h);

            auto velInAcc  = velInBuff.get_access<sycl::access::mode::read>(h);
            auto velOutAcc = velOutBuff.get_access<sycl::access::mode::write>(h);
            
           
            h.parallel_for(sycl::nd_range<2>({ gridWidth, gridHeight }, { 8,8 }), [=](sycl::nd_item<2> item) {

                size_t x = item.get_global_id(0);
                size_t y = item.get_global_id(1);

                size_t global_id = y * gridWidth + x; //Creates a global index space
                fungt::Vec3 p = posInAcc[global_id];
                fungt::Vec3 vel = velInAcc[global_id];
                // Pin a few top vertices (same pattern as GLSL)    
                if ((int)y == gridHeight - 1 && ((int)x == 0 || (int)x == gridWidth / 4 ||
                    (int)x == gridWidth / 2 || (int)x == 3 * gridWidth / 4 ||
                    (int)x == gridWidth - 1)) {

                    posOutAcc[global_id] = p;
                    velOutAcc[global_id] = vel;
                    return;
                }
                // Force accumulator
                float fx = 0.0f;
                float fy = 0.0f;
                float fz = 0.0f;

                fy += gravityY * invM * particleMass;
                fungt::Vec3 force{ 0.f, gravityY * particleMass, 0.f };

                // Structural springs (above/below)
                if (y < gridHeight - 1) {
                    fungt::Vec3 d = posInAcc[global_id + gridWidth] - p;
                    float len = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
                    if (len > 0.f) force += d * (k * (len - restV) / len);
                }
                if (y > 0) {
                    fungt::Vec3 d = posInAcc[global_id - gridWidth] - p;
                    float len = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
                    if (len > 0.f) force += d * (k * (len - restV) / len);
                }

                // Structural springs (left/right)
                if (x > 0) {
                    fungt::Vec3 d = posInAcc[global_id - 1] - p;
                    float len = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
                    if (len > 0.f) force += d * (k * (len - restH) / len);
                }
                if (x < gridWidth - 1) {
                    fungt::Vec3 d = posInAcc[global_id + 1] - p;
                    float len = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
                    if (len > 0.f) force += d * (k * (len - restH) / len);
                }

                // Shear springs (diagonals)
                if (x > 0 && y < gridHeight - 1) {
                    fungt::Vec3 d = posInAcc[global_id + gridWidth - 1] - p;
                    float len = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
                    if (len > 0.f) force += d * (k * (len - restD) / len);
                }
                if (x < gridWidth - 1 && y < gridHeight - 1) {
                    fungt::Vec3 d = posInAcc[global_id + gridWidth + 1] - p;
                    float len = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
                    if (len > 0.f) force += d * (k * (len - restD) / len);
                }
                if (x > 0 && y > 0) {
                    fungt::Vec3 d = posInAcc[global_id - gridWidth - 1] - p;
                    float len = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
                    if (len > 0.f) force += d * (k * (len - restD) / len);
                }
                if (x < gridWidth - 1 && y > 0) {
                    fungt::Vec3 d = posInAcc[global_id - gridWidth + 1] - p;
                    float len = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
                    if (len > 0.f) force += d * (k * (len - restD) / len);
                }

                // Damping
                force -= vel * damp;

                // Semi-implicit Euler integration
                fungt::Vec3 a = force * invM;
                fungt::Vec3 newVel = vel + a * dt;
                fungt::Vec3 newPos = p + newVel * dt + a * (0.5f * dt * dt);

                velOutAcc[global_id] = newVel;
                posOutAcc[global_id] = newPos;
                //What else here?

            });
        });//end queue
        kernel_completion_event.wait_and_throw();
        clFlush(clqueue);
        // FORCE SYNC: Just create host accessor to trigger sync, but don't copy
        {
            auto host_acc1 = outPosBuff.template get_access<sycl::access::mode::read>();
            
            // Just creating the accessor forces the sync back to OpenCL buffer
            // No need to actually use host_acc or copy data
            (void)host_acc1; // Suppress unused variable warning
          
        } 
    }   

    // CRITICAL: Memory barrier before releasing GL objects
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

    // Now release the GL objects back to OpenGL
    cl_mem cl_buffers_to_release[] = { cl_posInBuff, cl_posOutBuff };
    clEnqueueReleaseGLObjects(clqueue, 2, cl_buffers_to_release, 0, NULL, NULL);
    clFinish(clqueue);

    // Clean up OpenCL memory objects
    clReleaseMemObject(cl_posInBuff);
    clReleaseMemObject(cl_posOutBuff);
  
     // Swap which buffer is active for next frame
    currentBuffer = 1 - currentBuffer;

    // Update VAO to point to the new current buffer
    m_vao.bind();
    m_vbo[currentBuffer].bind();
    m_vao.unbind();

    // DON'T swap - just copy velocities
    std::copy(velOut.begin(), velOut.end(), velIn.begin());
    // CRITICAL: Swap buffers for next frame
    //std::swap(posIn, posOut);
    //std::swap(velIn, velOut);
}

void Clothing::draw()
{
    for(int i = 0; i < StepsPerFrame; ++i) {
        this->simulation();
    }
/*     glEnable(GL_PROGRAM_POINT_SIZE);
    m_vao.bind();
    glDrawArrays(GL_POINTS, 0, GridSize); */

    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(PRIM_RESTART);

    m_vao.bind();
    glDrawElements(GL_TRIANGLE_STRIP, numIndices, GL_UNSIGNED_INT, 0);
    m_vao.unbind();

    
}

Shader& Clothing::getShader()
{
    // TODO: insert return statement here
    return m_shader;
}

void Clothing::updateTime(float deltaTime)
{
    this->m_deltaTime = deltaTime;
}

void Clothing::setViewMatrix(const glm::mat4& viewMatrix)
{
    m_viewMatrix = viewMatrix;
}

glm::mat4 Clothing::getViewMatrix()
{
    return m_viewMatrix;
}

void Clothing::updateModelMatrix()
{

    m_ModelMatrix = glm::mat4(1.f);
    m_ModelMatrix = glm::translate(m_ModelMatrix, m_position);
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.x), glm::vec3(1.f, 0.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.y), glm::vec3(0.f, 1.f, 0.f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(m_rotation.z), glm::vec3(0.f, 0.f, 1.f));
    m_ModelMatrix = glm::scale(m_ModelMatrix, m_scale);
}

glm::mat4 Clothing::getModelMatrix() const
{
    return m_ModelMatrix;
}
