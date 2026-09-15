#include "vulkan_device.hpp"


void VulkanRenderDevice::setWindowHints() {
    // Vulkan-specific window hints can be set here if needed
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // No OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

}

void VulkanRenderDevice::createVulkanInstance()
{
    //Creatin vulkan instance
    VkApplicationInfo appInfo{};

    std::vector<const char*> layer = {
        "VK_LAYER_KHRONOS_validation"
    };
    std::vector<const char*> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        "VK_KHR_xcb_surface", // For Linux X11
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,

    }; 

    const VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "FunGT",
        .applicationVersion = VK_MAKE_API_VERSION(1, 0, 0),
        .pEngineName = "FunGT Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_3
    };

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(layer.size()),
        .ppEnabledLayerNames = layer.data(), //plugins for API 
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data()
    };

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
    CHECK_VK_RESULT(result, "Create Vulkan instance");
    std::couyt << "Vulkan instance created successfully." << std::endl;

}
void VulkanRenderDevice::createDebugCallback()
{
    VkDebugUtilsMessengerCreateInfoEXT MessengerCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = &DebugCallback,
        .pUserData = nullptr
    };



}
void VulkanRenderDevice::init(GLFWwindow* window, int width, int height, const float clearColor[4]) {
    glfwMakeContextCurrent(window);
#ifdef __APPLE__
    if (glfwInit() != GL_TRUE)
        std::cerr << "GLFW re-init error on Apple" << std::endl;
#else
    if (glewInit() != GLEW_OK)
        std::cerr << "GLEW init error" << std::endl;
#endif
    createVulkanInstance();
    // Additional Vulkan initialization code goes here

}