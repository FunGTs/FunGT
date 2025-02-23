cmake_minimum_required(VERSION 3.15)
project(FunGT)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(UNIX)
    # Set the build type to Release
    set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/release/linux)
endif()
if (WIN32)
    # Add the macro definition
    add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
    # Set vcpkg toolchain file
    set(CMAKE_TOOLCHAIN_FILE "C:/Users/juang/Documents/Development/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    # Set CMake prefix path for vcpkg packages
    set(CMAKE_PREFIX_PATH "C:/Users/juang/Documents/Development/vcpkg/installed/x64-windows")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL_VENDOR_NVIDIA")
    set(GLFW_BUILD_STATIC ON)
    #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
    message(STATUS "Working Dir: " ${CMAKE_SOURCE_DIR})
endif()
# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/../VertexGL
    ${CMAKE_SOURCE_DIR}/../Shaders
    ${CMAKE_SOURCE_DIR}/../funGT
    ${CMAKE_SOURCE_DIR}/../AnimatedModel
    ${CMAKE_SOURCE_DIR}/../Textures
    ${CMAKE_SOURCE_DIR}/../vendor/stb_image
    ${CMAKE_SOURCE_DIR}/../vendor/imgui
    ${CMAKE_SOURCE_DIR}/../Imgui_Setup
    ${CMAKE_SOURCE_DIR}/../Material
    ${CMAKE_SOURCE_DIR}/../Mesh
    ${CMAKE_SOURCE_DIR}/../Camera
    ${CMAKE_SOURCE_DIR}/../Geometries
    ${CMAKE_SOURCE_DIR}/../Model
    ${CMAKE_SOURCE_DIR}/../Helpers
    ${CMAKE_SOURCE_DIR}/../Animation
    ${CMAKE_SOURCE_DIR}/../Bone
    ${CMAKE_SOURCE_DIR}/../Matrix
    ${CMAKE_SOURCE_DIR}/../SceneManager
    ${CMAKE_SOURCE_DIR}/../CubeMap
    ${CMAKE_SOURCE_DIR}/../Physics/ParticleSystem
    ${CMAKE_SOURCE_DIR}/../Random
    ${CMAKE_SOURCE_DIR}/../Path_Manager
    ${CMAKE_SOURCE_DIR}/../InfoWindow
    ${CMAKE_SOURCE_DIR}/../GUI
)

# Source files
set(SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/../VertexGL/vertexArrayObjects.cpp
    ${CMAKE_SOURCE_DIR}/../VertexGL/vertexBuffers.cpp
    ${CMAKE_SOURCE_DIR}/../VertexGL/vertexIndices.cpp
    ${CMAKE_SOURCE_DIR}/../Shaders/shader.cpp
    ${CMAKE_SOURCE_DIR}/../funGT/fungt.cpp
    ${CMAKE_SOURCE_DIR}/../AnimatedModel/animated_model.cpp
    ${CMAKE_SOURCE_DIR}/../Textures/textures.cpp
    ${CMAKE_SOURCE_DIR}/../vendor/stb_image/stb_image.cpp
    ${CMAKE_SOURCE_DIR}/../vendor/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/../vendor/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/../vendor/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/../vendor/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/../vendor/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/../vendor/imgui/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/../vendor/imgui/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/../Imgui_Setup/imgui_setup.cpp
    ${CMAKE_SOURCE_DIR}/../Material/material.cpp
    ${CMAKE_SOURCE_DIR}/../Mesh/mesh.cpp
    ${CMAKE_SOURCE_DIR}/../Camera/camera.cpp
    ${CMAKE_SOURCE_DIR}/../Geometries/cube.cpp
    ${CMAKE_SOURCE_DIR}/../Geometries/plane.cpp
    ${CMAKE_SOURCE_DIR}/../Geometries/primitives.cpp
    ${CMAKE_SOURCE_DIR}/../Geometries/pyramid.cpp
    ${CMAKE_SOURCE_DIR}/../Geometries/shape.cpp
    ${CMAKE_SOURCE_DIR}/../Geometries/square.cpp
    ${CMAKE_SOURCE_DIR}/../Model/model.cpp
    ${CMAKE_SOURCE_DIR}/../Helpers/helpers.cpp
    ${CMAKE_SOURCE_DIR}/../Animation/animation.cpp
    ${CMAKE_SOURCE_DIR}/../Bone/bone.cpp
    ${CMAKE_SOURCE_DIR}/../Matrix/matrix4x4f.cpp
    ${CMAKE_SOURCE_DIR}/../SceneManager/scene_manager.cpp
    ${CMAKE_SOURCE_DIR}/../CubeMap/cube_map.cpp
    ${CMAKE_SOURCE_DIR}/../Physics/ParticleSystem/particle_sys.cpp
    ${CMAKE_SOURCE_DIR}/../Physics/ParticleSystem/particle.cpp
    ${CMAKE_SOURCE_DIR}/../Random/random.cpp
    ${CMAKE_SOURCE_DIR}/../Path_Manager/path_manager.cpp
    ${CMAKE_SOURCE_DIR}/../InfoWindow/infowindow.cpp
    ${CMAKE_SOURCE_DIR}/../GUI/gui.cpp
)

# Add executable
add_executable(FunGT ${SOURCE_FILES})

# Platform-specific configurations
if (WIN32)
    # Use vcpkg packages on Windows
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(glfw3  REQUIRED)
    find_package(assimp REQUIRED)
    find_package(glm REQUIRED)
    target_link_libraries(FunGT
        PRIVATE
        OpenGL::GL 
        glfw
        GLEW::GLEW
        assimp::assimp
        glm::glm
    )
elseif (UNIX)
    # Linux-specific settings
    message(STATUS "Configuring for Linux")

    find_package(OpenGL REQUIRED)
    if (OpenGL_FOUND)
        message(STATUS "OpenGL found")
    else()
        message(FATAL_ERROR "OpenGL library not found!")
    endif()

    find_package(glfw3 REQUIRED)
    if (glfw3_FOUND)
        message(STATUS "GLFW found")
    else()
        message(FATAL_ERROR "GLFW library not found!")
    endif()

    find_package(GLEW REQUIRED)
    if (GLEW_FOUND)
        message(STATUS "GLEW found")
    else()
        message(FATAL_ERROR "GLEW library not found!")
    endif()

    find_package(assimp REQUIRED)
    if (assimp_FOUND)
        message(STATUS "Assimp found")
    else()
        message(FATAL_ERROR "Assimp library not found!")
    endif()

    target_link_libraries(FunGT
        PRIVATE
        OpenGL::GL
        glfw
        GLEW::GLEW
        assimp::assimp
        dl
    )
endif()