cmake_minimum_required(VERSION 3.15)
project(pbr_render)

option(FUNGT_USE_CUDA "Enable CUDA backend" OFF)
option(FUNGT_USE_SYCL "Enable SYCL backend" OFF)
# Allow FUNGT_BASE_DIR to be set externally (command line, environment, or cache)
# Priority: 1. Cache variable, 2. Environment variable, 3. Default fallback
if(NOT DEFINED FUNGT_BASE_DIR)
    # Check if it's set as an environment variable
    if(DEFINED ENV{FUNGT_BASE_DIR})
        set(FUNGT_BASE_DIR $ENV{FUNGT_BASE_DIR} CACHE PATH "FunGT base directory")
        message(STATUS "Using FUNGT_BASE_DIR from environment: ${FUNGT_BASE_DIR}")
    else()
        # Use a default fallback (you must specify this via -DFUNGT_BASE_DIR or environment)
        message(FATAL_ERROR "FUNGT_BASE_DIR not specified. Please set it via:\n"
                "  cmake -DFUNGT_BASE_DIR=/path/to/FunGT ..\n"
                "  or export FUNGT_BASE_DIR=/path/to/FunGT")
    endif()
else()
    message(STATUS "Using pre-defined FUNGT_BASE_DIR: ${FUNGT_BASE_DIR}")
endif()
# Validate that the base directory exists and contains expected subdirectories
if(NOT EXISTS "${FUNGT_BASE_DIR}")
    message(FATAL_ERROR "FUNGT_BASE_DIR does not exist: ${FUNGT_BASE_DIR}")
endif()

if(NOT EXISTS "${FUNGT_BASE_DIR}/VertexGL" OR NOT EXISTS "${FUNGT_BASE_DIR}/Shaders")
    message(FATAL_ERROR "FUNGT_BASE_DIR does not appear to be a valid FunGT directory: ${FUNGT_BASE_DIR}")
endif()
 # Set the build type to Release
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/release/linux)
set(CMAKE_CXX_COMPILER /home/juanchuletas/Documents/Development/sycl_workspace/llvm/build/bin/clang++)
# Add funlib
set(FUNLIB_DIR ${FUNGT_BASE_DIR}/vendor/funlib)
add_library(funlib STATIC IMPORTED GLOBAL)
set_target_properties(funlib PROPERTIES
    IMPORTED_LOCATION ${FUNLIB_DIR}/lib/libfunlib.a
    INTERFACE_INCLUDE_DIRECTORIES ${FUNLIB_DIR}/include
)

#include directories
include_directories(
    ${FUNGT_BASE_DIR}/Vector
    ${FUNGT_BASE_DIR}/Triangle
    ${FUNGT_BASE_DIR}/Material
    ${FUNGT_BASE_DIR}/VertexGL
    ${FUNGT_BASE_DIR}/Shader
    ${FUNGT_BASE_DIR}/Random
    ${FUNGT_BASE_DIR}/SimpleModel/
    ${FUNGT_BASE_DIR}/Model/
    ${FUNGT_BASE_DIR}/Mesh/
    ${FUNGT_BASE_DIR}/Path_Manager
    ${FUNGT_BASE_DIR}/PBR/PBRCamera
    ${FUNGT_BASE_DIR}/PBR/Ray
    ${FUNGT_BASE_DIR}/PBR/HitData
    ${FUNGT_BASE_DIR}/PBR/Intersection
    ${FUNGT_BASE_DIR}/PBR/Light
    ${FUNGT_BASE_DIR}/PBR/Space
    ${FUNGT_BASE_DIR}/PBR/Render/include
    ${FUNGT_BASE_DIR}/vendor/stb_image
    ${FUNGT_BASE_DIR}/Textures
    ${FUNGT_BASE_DIR}/Renders
    ${FUNGT_BASE_DIR}/Matrix
    ${FUNGT_BASE_DIR}/gpu/data
    ${FUNGT_BASE_DIR}/gpu/include
)
set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${FUNGT_BASE_DIR}/PBR/Intersection/intersection.cpp
    ${FUNGT_BASE_DIR}/PBR/Space/space.cpp
    ${FUNGT_BASE_DIR}/Material/material.cpp
    ${FUNGT_BASE_DIR}/Shaders/shader.cpp
    ${FUNGT_BASE_DIR}/Random/random.cpp
    ${FUNGT_BASE_DIR}/SimpleModel/simple_model.cpp
    ${FUNGT_BASE_DIR}/Model/model.cpp
    ${FUNGT_BASE_DIR}/Path_Manager/path_manager.cpp
    ${FUNGT_BASE_DIR}/Mesh/mesh.cpp
    ${FUNGT_BASE_DIR}/Textures/textures.cpp
    ${FUNGT_BASE_DIR}/vendor/stb_image/stb_image.cpp
    ${FUNGT_BASE_DIR}/VertexGL/vertexArrayObjects.cpp
    ${FUNGT_BASE_DIR}/VertexGL/vertexBuffers.cpp
    ${FUNGT_BASE_DIR}/VertexGL/vertexIndices.cpp
    ${FUNGT_BASE_DIR}/Matrix/matrix4x4f.cpp
    ${FUNGT_BASE_DIR}/Matrix/matrix3x3f.cpp
    ${FUNGT_BASE_DIR}/Renders/display_graphics.cpp
    ${FUNGT_BASE_DIR}/PBR/Render/src/compute_backends.cpp
)
# ────────────────────────────────
# Conditional CUDA setup
# ────────────────────────────────
if(FUNGT_USE_CUDA)
    message(STATUS "CUDA backend enabled")
    
    enable_language(CUDA)
    list(APPEND SOURCE_FILES ${FUNGT_BASE_DIR}/PBR/Render/src/cuda_renderer.cu)

    set_source_files_properties(${FUNGT_BASE_DIR}/PBR/Render/src/cuda_renderer.cu PROPERTIES LANGUAGE CUDA)
      # Add CUDA-specific compile options
      
else()
    message(STATUS "CUDA backend disabled")
endif()
# Add executable
add_executable(pbr_render ${SOURCE_FILES})
# ────────────────────────────────
# CUDA target settings (only after the target exists)
# ────────────────────────────────
if(FUNGT_USE_CUDA)
   target_compile_definitions(pbr_render PRIVATE FUNGT_USE_CUDA)
    set_target_properties(pbr_render PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        CUDA_ARCHITECTURES "native"
    )
endif()
# Linux-specific settings
message(STATUS "Configuring for Linux")
message(STATUS "CMakeLists.txt location: ${FUNGT_BASE_DIR}")
find_package(OpenCL REQUIRED)
if (OpenCL_FOUND)
    message(STATUS "OpenCL found")
    message(STATUS "OpenCL include dirs: ${OpenCL_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "OpenCL library not found!")
endif()
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
find_package(glm CONFIG REQUIRED)
if(glm_FOUND)
    message(STATUS "glm found")
else()
    message(FATAL_ERROR "glm not found!")
endif()
target_include_directories(pbr_render PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(pbr_render
    PRIVATE
    OpenGL::GL
    glfw
    GLEW::GLEW
    assimp::assimp
    glm::glm
    dl
    funlib
    ${OpenCL_LIBRARIES}
)
# For SYCL build path
if(FUNGT_USE_SYCL AND NOT FUNGT_USE_CUDA)
    target_compile_options(pbr_render PRIVATE -fsycl)
    target_link_options(pbr_render PRIVATE -fsycl)
else()
    #target_compile_options(pbr_render PRIVATE --diag-suppress=20012)
endif()