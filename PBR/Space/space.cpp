#include "space.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../vendor/stb_image/stb_image_write.h"
Space::Space(){
    m_lights.push_back(Light(
        fungt::Vec3(-5.0f, 8.0f, 4.0f),    // position
        fungt::Vec3(10.0f, 10.0f, 10.0f)  // strong white intensity
    ));  
    // DEBUG
    std::cout << "DEBUG: GetBackend() returns: " << static_cast<int>(ComputeRender::GetBackend()) << std::endl;
    std::cout << "  CUDA = " << static_cast<int>(Compute::Backend::CUDA) << std::endl;
    std::cout << "  SYCL = " << static_cast<int>(Compute::Backend::SYCL) << std::endl;
    std::cout << "  CPU = " << static_cast<int>(Compute::Backend::CPU) << std::endl;
    switch (ComputeRender::GetBackend())
    {
    case Compute::Backend::CPU:
    {
        /* code */
        std::cout << "Using CPU to render scene" << std::endl;
        m_computeRenderer = std::make_unique<CPU_Renderer>();
        m_textureManager = std::make_shared<CPUTexture>();
        break;
    }
#ifdef FUNGT_USE_CUDA
    case Compute::Backend::CUDA:
    {
        /* code */
        std::cout << "Using CUDA to render scene" << std::endl;
        m_computeRenderer = std::make_unique<CUDA_Renderer>();

        break;
    }
#endif
#ifdef FUNGT_USE_SYCL
    case Compute::Backend::SYCL:
    {
        std::cout << "Using SYCL to render scene" << std::endl;
        m_computeRenderer = std::make_unique<SYCL_Renderer>();
      
        break;
    }
#endif
    default:
        throw std::runtime_error("Unknown Compute API!");
    }

}

Space::Space(std::vector<Triangle>& triangleList)
{
    Material gray(glm::vec3(0.2f), glm::vec3(0.8f), glm::vec3(0.8f), 32.0f, "DefaultGray");
    m_triangles = std::move(triangleList);
    m_lights.push_back(Light(
        fungt::Vec3(2.0f, 2.0f, 2.0f),    // position
        fungt::Vec3(10.0f, 10.0f, 10.0f)  // strong white intensity
    ));
}

Space::Space(const PBRCamera& camera)
: Space() {
    m_camera = camera;
}


Space::~Space(){
}

std::vector<fungt::Vec3> Space::Render(const int width, const int height) {
  
    std::cout << "Starting render with " << ComputeRender::GetBackendName() << std::endl;
    size_t triMem = m_triangles.size() * sizeof(Triangle);
    size_t bvhMem = m_bvh_nodes.size() * sizeof(BVHNode);
    size_t lightMem = m_lights.size() * sizeof(Light);
    size_t frameMem = (width*height) * sizeof(fungt::Vec3);
    size_t totalMem = triMem + bvhMem + lightMem + frameMem;

    std::cout << "Memory usage:\n"
        << "  Triangles: " << triMem / (1024.0 * 1024.0) << " MB\n"
        << "  BVH nodes: " << bvhMem / (1024.0 * 1024.0) << " MB\n"
        << "  Lights:    " << lightMem / (1024.0 * 1024.0) << " MB\n"
        << "  Framebuffer: " << frameMem / (1024.0 * 1024.0) << " MB\n"
        << "  Total:     " << totalMem / (1024.0 * 1024.0) << " MB\n";
    std::vector<fungt::Vec3> frameBuffer = m_computeRenderer->RenderScene(
        width, height, m_triangles, m_bvh_nodes, m_lights, m_camera, m_samplesPerPixel
    );

    return frameBuffer;
}

void Space::sendTexturesToRender()
{
    if (!m_computeRenderer) {
        std::cout << "Error in sendTexturesToRender :  computeRenderer pointer is null" <<std::endl;
        return;
    }
    switch (ComputeRender::GetBackend())
    {
    case Compute::Backend::CPU:
        std::cout << "Sending CPU Textures" << std::endl;
        
        break;

#ifdef FUNGT_USE_CUDA
    case Compute::Backend::CUDA:
    {
        std::cout << "Sending CUDA Textures" << std::endl;
        
        // Set textures on renderer
        CUDA_Renderer* cudaRenderer = dynamic_cast<CUDA_Renderer*>(m_computeRenderer.get());
        if (cudaRenderer && m_textureManager) {
            auto cudaTexMgr = dynamic_cast<CUDATexture*>(m_textureManager.get());
            if (cudaTexMgr)
                cudaRenderer->setCudaTextureObjects(cudaTexMgr->getTextureObjects());
        }
        break;
    }
#endif

#ifdef FUNGT_USE_SYCL
    case Compute::Backend::SYCL:
    {
        std::cout << "Sending SYCL Textures" << std::endl;
        SYCL_Renderer* syclRenderer = dynamic_cast<SYCL_Renderer*>(m_computeRenderer.get());
        if (syclRenderer && m_textureManager) { 
            // Set textures on renderer
            auto syclTexMgr = dynamic_cast<SYCLTexture*>(m_textureManager.get());
            if (syclTexMgr)
                syclRenderer->setSyclTextureHandles(syclTexMgr->getImageHandles());
        }
        break;
    }
#endif

    default:
        throw std::runtime_error("Unknown Compute API!");
    }

}

void Space::InitComputeRenderBackend()
{
    if (!m_computeRenderer) {
        std::cout << "Error: Renderer not created!" << std::endl;
        return;
    }

    switch (ComputeRender::GetBackend())
    {
    case Compute::Backend::CPU:
        std::cout << "Initializing CPU backend" << std::endl;
        m_textureManager = std::make_shared<CPUTexture>();
        break;

#ifdef FUNGT_USE_CUDA
    case Compute::Backend::CUDA:
    {
        std::cout << "Initializing CUDA backend" << std::endl;
        m_textureManager = std::make_shared<CUDATexture>();

        break;
    }
#endif

#ifdef FUNGT_USE_SYCL
    case Compute::Backend::SYCL:
    {
        std::cout << "Initializing SYCL backend" << std::endl;
        SYCL_Renderer* syclRenderer = dynamic_cast<SYCL_Renderer*>(m_computeRenderer.get());
        if (syclRenderer) {
            syclRenderer->createQueue();
            m_textureManager = std::make_shared<SYCLTexture>(syclRenderer->getQueue());
        }
        break;
    }
#endif

    default:
        throw std::runtime_error("Unknown Compute API!");
    }
}

void Space::LoadModelToRender(const SimpleModel& Simplemodel)
{
    Model& model = Simplemodel.getModel();
    const std::vector<std::unique_ptr<Mesh>>& meshes = model.getMeshes();
    size_t totalTriangles = 0;


    for (const auto& meshPtr : meshes) {

        totalTriangles += meshPtr->m_index.size() / 3;
    }
    std::cout << "Total triangles to create: " << totalTriangles << std::endl;
    m_triangles.reserve(totalTriangles);

    for (auto& meshPtr : meshes) {
        std::cout << " ********* Processing Mesh *********** : " << meshPtr->m_index.size() << std::endl;
        const auto& vertices = meshPtr->m_vertex;
        const auto& indices = meshPtr->m_index;
        const auto& materials = meshPtr->m_material;
        const auto& textures = meshPtr->m_texture;

        std::cout << "Vertices: " << vertices.size() << std::endl;
        std::cout << "Indices: " << indices.size() << std::endl;
        std::cout << "Textures: " << textures.size() << std::endl;
        std::cout << "Materials: " << materials.size() << std::endl;

        // ========== Setup Material ONCE per mesh ==========
        MaterialData global_material;
        global_material.baseColor[0] = 0.922;
        global_material.baseColor[1] = 0.467;
        global_material.baseColor[2] = 0.882f;
        global_material.metallic = 0.0f;
        global_material.roughness = 0.5f;
        global_material.reflectance = 0.05f;
        global_material.emission = 0.0f;
        global_material.baseColorTexIdx = -1;
        if (!textures.empty() && m_textureManager != nullptr) {
            std::string texPath = textures[0].getPath();
            std::cout << "  Loading texture: " << texPath << std::endl;
            global_material.baseColorTexIdx = m_textureManager->loadTexture(texPath);
            std::cout << "  Assigned texture index: " << global_material.baseColorTexIdx << std::endl;
        }
        else {
            std::cout << "  No texture (using base color)" << std::endl;
        }
        if (!materials.empty()) {
            global_material.baseColor[0] = materials[0].m_diffLigth.x;
            global_material.baseColor[1] = materials[0].m_diffLigth.y;
            global_material.baseColor[2] = materials[0].m_diffLigth.z;

            float Ns = materials[0].m_shininess;
            global_material.roughness = sqrtf(2.0f / (Ns + 2.0f));

            float avgSpec = (materials[0].m_specLight.x +
                materials[0].m_specLight.y +
                materials[0].m_specLight.z) / 3.0f;
            global_material.metallic = (avgSpec < 0.9f) ? 0.0f : 0.3f;

            std::cout << "Material: Base color ("
                << global_material.baseColor[0] << ", "
                << global_material.baseColor[1] << ", "
                << global_material.baseColor[2] << ")" << std::endl;
            std::cout << "  Roughness: " << global_material.roughness << std::endl;
            std::cout << "  Metallic: " << global_material.metallic << std::endl;
        }
        for (size_t i = 0; i < meshPtr->m_index.size(); i += 3) {
            const funGTVERTEX& v0 = meshPtr->m_vertex[meshPtr->m_index[i + 0]];
            const funGTVERTEX& v1 = meshPtr->m_vertex[meshPtr->m_index[i + 1]];
            const funGTVERTEX& v2 = meshPtr->m_vertex[meshPtr->m_index[i + 2]];
            Triangle tri;

            tri.v0 = fungt::toFungtVec3(v0.position);
            tri.v1 = fungt::toFungtVec3(v1.position);
            tri.v2 = fungt::toFungtVec3(v2.position);

            // Flat face normal (correct for path tracing)
            // fungt::Vec3 e1 = tri.v1 - tri.v0;
            // fungt::Vec3 e2 = tri.v2 - tri.v0;
            // tri.normal = e1.cross(e2).normalize();

            // Per-vertex normals (for smooth shading!)
            tri.n0 = fungt::toFungtVec3(v0.normal);
            tri.n1 = fungt::toFungtVec3(v1.normal);
            tri.n2 = fungt::toFungtVec3(v2.normal);

            tri.uvs[0][0] = v0.texcoord.x;
            tri.uvs[0][1] = v0.texcoord.y;
            tri.uvs[1][0] = v1.texcoord.x;
            tri.uvs[1][1] = v1.texcoord.y;
            tri.uvs[2][0] = v2.texcoord.x;
            tri.uvs[2][1] = v2.texcoord.y;
            // Assign material (same for all triangles in this mesh)
            tri.material = global_material;

            m_triangles.push_back(std::move(tri));
        }
        std::cout << "Material: Base color ("
            << global_material.baseColor[0] << ", "
            << global_material.baseColor[1] << ", "
            << global_material.baseColor[2] << ")" << std::endl;
        std::cout << "  Roughness: " << global_material.roughness << std::endl;
        std::cout << "  Metallic: " << global_material.metallic << std::endl;
    }


    sendTexturesToRender();
}

void Space::SaveFrameBufferAsPNG(const std::vector<fungt::Vec3>& framebuffer, int width, int height)
{
    std::vector<unsigned char> pixels(width * height * 3);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Flip Y coordinate
            int srcIdx = y * width + x;              // Original index
            int dstIdx = (height - 1 - y) * width + x;  // Flipped index

            fungt::Vec3 color = framebuffer[srcIdx];

            // Clamp and gamma correct
            color.x = std::pow(std::clamp(color.x, 0.0f, 1.0f), 1.0f / 2.2f);
            color.y = std::pow(std::clamp(color.y, 0.0f, 1.0f), 1.0f / 2.2f);
            color.z = std::pow(std::clamp(color.z, 0.0f, 1.0f), 1.0f / 2.2f);

            pixels[dstIdx * 3 + 0] = static_cast<unsigned char>(255.99f * color.x);
            pixels[dstIdx * 3 + 1] = static_cast<unsigned char>(255.99f * color.y);
            pixels[dstIdx * 3 + 2] = static_cast<unsigned char>(255.99f * color.z);
        }
    }

    std::string file_name = ComputeRender::GetBackendName() + "_render" + "_output.png";
    stbi_write_png(file_name.c_str(), width, height, 3, pixels.data(), width * 3);
}

void Space::BuildBVH()
{
    
    BVHBuilder builder;
    builder.build(m_triangles);

    m_bvh_nodes   = builder.moveNodes();
    m_bvh_indices = builder.moveIndices();
    std::vector<Triangle> reordered(m_triangles.size());
    for (size_t i = 0; i < m_bvh_indices.size(); i++) {
        reordered[i] = m_triangles[m_bvh_indices[i]];
    }
    m_triangles = std::move(reordered);
}

void Space::setSamples(int numOfSamples)
{
    m_samplesPerPixel = numOfSamples;
}



