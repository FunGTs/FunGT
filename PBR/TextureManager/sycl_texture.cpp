    #include "sycl_texture.hpp"
    #include "stb_image.h"

    SYCLTexture::SYCLTexture(sycl::queue& queue)
    :m_queue{&queue}{

        std::cout << "SYCLTexture: Initialized with queue for device: "
            << queue.get_device().get_info<sycl::info::device::name>()
            << std::endl;

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
                syclexp::create_image(imgMem, sampler,desc, *m_queue);

            SYCLTextureData texData;
            texData.imgHandle = imgHandle;
            texData.imgMem = std::move(imgMem);
            texData.width = width;
            texData.height = height;
            texData.path = path;
            int index = textures.size();
            textures.push_back(std::move(texData));
            pathToIndex[path] = index;

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

            std::cout << "SYCLTexture: Cleanup complete" << std::endl;
        }
        catch (const sycl::exception& e) {
            std::cerr << "SYCLTexture: Queue wait failed: " << e.what() << std::endl;
        }

        textures.clear();
        pathToIndex.clear();
    }
