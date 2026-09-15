cmake_minimum_required(VERSION 3.10)

# Use your SYCL clang++
set(CMAKE_CXX_COMPILER /home/juanchuletas/Documents/Development/sycl_workspace/llvm/build/bin/clang++)

project(imgui)

# Core ImGui sources
set(IMGUI_SOURCES
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_impl_glfw.cpp
    imgui_impl_opengl3.cpp
)

# Make sure OpenGL uses GLVND
cmake_policy(SET CMP0072 NEW)
set(OpenGL_GL_PREFERENCE GLVND)

# Find dependencies
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan QUIET)

# Add Vulkan backend if available
if(Vulkan_FOUND)
    list(APPEND IMGUI_SOURCES imgui_impl_vulkan.cpp)
    message(STATUS "ImGui: Vulkan backend enabled")
else()
    message(STATUS "ImGui: Vulkan not found, building without Vulkan backend")
endif()

# Create a static library
add_library(imgui STATIC ${IMGUI_SOURCES})

# Include ImGui headers
target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set output directory for the static library
set_target_properties(imgui PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)

# Link dependencies
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

if(Vulkan_FOUND)
    target_link_libraries(imgui PUBLIC Vulkan::Vulkan)
    target_compile_definitions(imgui PUBLIC IMGUI_VULKAN_BACKEND_AVAILABLE)
endif()
