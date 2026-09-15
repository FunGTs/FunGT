#if !defined(_VULKAN_RENDER_DEVICE_HPP_)
#define _VULKAN_RENDER_DEVICE_HPP_
#include <vulkan/vulkan.h>
#include "vulkan_util.hpp"
class VulkanRenderDevice : public GraphicsRenderDevice {

private:
    
    VkInstance m_instance = nullptr;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

    void createVulkanInstance();
    void createDebugCallback();

public: 
    void init(GLFWwindow* window, int width, int height, const float clearColor[4]) override;
    void shutdown() override;
    void beginFrame() override;
    void endFrame(GLFWwindow* window) override;

    void setDepthMask(bool write) override;
    void setDepthFunc(DepthFunc func) override;
    void setBlendFunc(BlendFactor src, BlendFactor dst) override;

    ~VulkanRenderDevice() override {
        if (m_instance) {
            vkDestroyInstance(m_instance, nullptr);
        }
    }

    
};
#endif // _VULKAN_RENDER_DEVICE_HPP_
