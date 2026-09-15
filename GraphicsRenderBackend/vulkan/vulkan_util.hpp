#if !defined(_VULKAN_UTIL_HPP_)
#define _VULKAN_UTIL_HPP_

#define CHECK_VK_RESULT(res,msg) { \
  if(res != VK_SUCCESS) { \
    std::cerr << "\033[1;31mVulkan Error: " << res << " in file " << __FILE__ << " at line " << __LINE__ << ": " << msg << "\033[0m" << std::endl; \
    exit(0); \
  }
}


#endif // _VULKAN_UTIL_HPP_

