    #include "sycl_texture.hpp"
    #include "stb_image.h"

    SYCLTexture::SYCLTexture(sycl::queue& queue)
    :m_queue{&queue}{

        auto currDevice = m_queue.get_device();
        std::cout << "SYCLTexture: Initialized with queue for device: "
            << currDevice.get_info<sycl::info::device::name>()
            << std::endl;
        
        m_useBindlessImages = currDevice.has(sycl::aspect::ext_oneapi_bindless_images);
        std::cout << "Bindless Image support: " << (m_useBindlessImages ? "YES" : "NO") << std::endl;
    }
    SYCLTexture::~SYCLTexture() {
        cleanup();
    }

    int SYCLTexture::loadTexture(const std::string& path)
    {
        if (pathToIndex.find(path) != pathToIndex.end()) {
            std::cout << "SYCLTexture: Texture already loaded: " << path << std::endl;
            return pathToIndex[path];
        }
        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

        if (!data) {
            std::cerr << "SYCLTexture: Failed to load texture: " << path << std::endl;
            return -1;
        }
        std::cout << "SYCLTexture: Loaded " << path
            << " (" << width << "x" << height << ", " << channels << " channels)"
            << std::endl;
        try{
           

            int index = -1;

            if (m_useBindlessImages) {
                index = loadBindlessTexture(data, width, height, path);
            }
            else {
                index = loadBufferTexture(data, width, height, path);
            }

            stbi_image_free(data);

            std::cout << "SYCLTexture: Successfully loaded texture " << index
                << " (" << path << ")" << std::endl;

            return index;

        }
        catch(const std::exception& e)
        {
            std::cerr << "SYCLTexture: SYCL exception while loading " << path
                << ": " << e.what() << std::endl;
            stbi_image_free(data);
            return -1;
        }
        
    }

    void SYCLTexture::cleanup() {
        std::cout << "SYCLTexture: Cleaning up " << textures.size() << " textures" << std::endl;

        // Check if queue is still valid
        if (!m_queue) {
            std::cout << "SYCLTexture: Queue already destroyed, skipping cleanup" << std::endl;
            textures.clear();
            pathToIndex.clear();
            return;
        }

        try {
            // Wait for all operations to complete
            m_queue->wait_and_throw();

            if(m_useBindlessImages){
                for (size_t i = 0; i < textures.size(); i++) {
                    auto& tex = textures[i];
                    std::cout << "SYCLTexture: Destroying texture " << i << std::endl;

                    try {
                        syclexp::destroy_image_handle(tex.imgHandle, *m_queue);
                    }
                    catch (const sycl::exception& e) {
                        std::cerr << "SYCLTexture: Error destroying texture " << i
                            << ": " << e.what() << std::endl;
                    }
                }
            }
            else{
                for (auto& tex : m_bufferTextures) {
                    std::cout << "SYCLTexture: Destroying buffer texture " << i << std::endl;
                    sycl::free(tex.deviceData, *m_queue);
                }
            }

            std::cout << "SYCLTexture: Cleanup complete" << std::endl;
        }
        catch (const sycl::exception& e) {
            std::cerr << "SYCLTexture: Queue wait failed: " << e.what() << std::endl;
        }

        textures.clear();
        pathToIndex.clear();
    }

    int SYCLTexture::loadBindlessTexture(unsigned char* data, int width, int height, const std::string& path)
    {
        const unsigned int numChannels = 4;
        const auto channelType = sycl::image_channel_type::unorm_int8;
        syclexp::image_descriptor desc(
            { static_cast<size_t>(width), static_cast<size_t>(height) },
            numChannels,
            channelType
        );
        syclexp::image_mem imgMem(desc, *m_queue);

        auto cpyToDeviceEvent = m_queue->ext_oneapi_copy(
            data, //Source
            imgMem.get_handle(), //Destination
            desc //Image descriptor
        );
        cpyToDeviceEvent.wait_and_throw();
        syclexp::bindless_image_sampler sampler(
            sycl::addressing_mode::repeat,
            sycl::coordinate_normalization_mode::normalized,
            sycl::filtering_mode::linear
        );
        syclexp::sampled_image_handle imgHandle =
            syclexp::create_image(imgMem, sampler, desc, *m_queue);

        SYCLTextureData texData;
        texData.imgHandle = imgHandle;
        texData.imgMem = std::move(imgMem);
        texData.width = width;
        texData.height = height;
        texData.path = path;
        int index = textures.size();
        textures.push_back(std::move(texData));
        pathToIndex[path] = index;

        return index;
    }

    int SYCLTexture::loadBufferTexture(unsigned char* data, int w, int h, const std::string& path)
    {
        std::vector<float> floatData(w * h * 4);
        for (int i = 0; i < w * h * 4; i++) {
            floatData[i] = data[i] / 255.0f;
        }

        float* devData = sycl::malloc_device<float>(w * h * 4, *m_queue); //m_queue is of type pointer
        m_queue->memcpy(devData, floatData.data(), w * h * 4 * sizeof(float)).wait();


        BufferTextureData tex;
        tex.deviceData = devData;
        tex.width = w;
        tex.height = h;
        tex.path = path;

        
        int index = m_bufferTextures.size();
        m_bufferTextures.push_back(tex);

        std::cout << "Loaded buffer texture " << index << " (" << path << ")" << std::endl;
        return index;
    }
