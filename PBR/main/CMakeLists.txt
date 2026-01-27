cmake_minimum_required(VERSION 3.15)
project(pbr_render)

option(FUNGT_USE_CUDA "Enable CUDA backend" ON)
option(FUNGT_USE_SYCL "Enable SYCL backend" ON)

# ────────────────────────────────────────────────────────
# FUNGT_BASE_DIR validation
# ────────────────────────────────────────────────────────
if(NOT DEFINED FUNGT_BASE_DIR)
    if(DEFINED ENV{FUNGT_BASE_DIR})
        set(FUNGT_BASE_DIR $ENV{FUNGT_BASE_DIR} CACHE PATH "FunGT base directory")
        message(STATUS "Using FUNGT_BASE_DIR from environment: ${FUNGT_BASE_DIR}")
    else()
        message(FATAL_ERROR "FUNGT_BASE_DIR not specified. Please set it via:\n"
                "  cmake -DFUNGT_BASE_DIR=/path/to/FunGT ..\n"
                "  or export FUNGT_BASE_DIR=/path/to/FunGT")
    endif()
else()
    message(STATUS "Using pre-defined FUNGT_BASE_DIR: ${FUNGT_BASE_DIR}")
endif()

if(NOT EXISTS "${FUNGT_BASE_DIR}")
    message(FATAL_ERROR "FUNGT_BASE_DIR does not exist: ${FUNGT_BASE_DIR}")
endif()

if(NOT EXISTS "${FUNGT_BASE_DIR}/VertexGL" OR NOT EXISTS "${FUNGT_BASE_DIR}/Shaders")
    message(FATAL_ERROR "FUNGT_BASE_DIR does not appear to be a valid FunGT directory: ${FUNGT_BASE_DIR}")
endif()

# ────────────────────────────────────────────────────────
# Build configuration
# ────────────────────────────────────────────────────────
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/release/linux)

# Use Intel SYCL compiler (clang) for everything
set(CMAKE_CXX_COMPILER /home/juanchuletas/Documents/Development/sycl_workspace/llvm/build/bin/clang++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ────────────────────────────────────────────────────────
# funlib setup
# ────────────────────────────────────────────────────────
set(FUNLIB_DIR ${FUNGT_BASE_DIR}/vendor/funlib)
add_library(funlib STATIC IMPORTED GLOBAL)
set_target_properties(funlib PROPERTIES
    IMPORTED_LOCATION ${FUNLIB_DIR}/lib/libfunlib.a
    INTERFACE_INCLUDE_DIRECTORIES ${FUNLIB_DIR}/include
)

# ────────────────────────────────────────────────────────
# Include directories (shared by all targets)
# ────────────────────────────────────────────────────────
set(FUNGT_INCLUDES
    ${FUNGT_BASE_DIR}
    ${FUNGT_BASE_DIR}/Vector
    ${FUNGT_BASE_DIR}/Triangle
    ${FUNGT_BASE_DIR}/Material
    ${FUNGT_BASE_DIR}/VertexGL
    ${FUNGT_BASE_DIR}/Shader
    ${FUNGT_BASE_DIR}/Random
    ${FUNGT_BASE_DIR}/SimpleModel
    ${FUNGT_BASE_DIR}/SimpleGeometry
    ${FUNGT_BASE_DIR}/Model
    ${FUNGT_BASE_DIR}/Mesh
    ${FUNGT_BASE_DIR}/Path_Manager
    ${FUNGT_BASE_DIR}/Geometries
    ${FUNGT_BASE_DIR}/PBR/PBRCamera
    ${FUNGT_BASE_DIR}/PBR/Ray
    ${FUNGT_BASE_DIR}/PBR/HitData
    ${FUNGT_BASE_DIR}/PBR/Intersection
    ${FUNGT_BASE_DIR}/PBR/Light
    ${FUNGT_BASE_DIR}/PBR/Space
    ${FUNGT_BASE_DIR}/PBR/Render/include
    ${FUNGT_BASE_DIR}/PBR/Render/brdf
    ${FUNGT_BASE_DIR}/PBR/Render/shared
    ${FUNGT_BASE_DIR}/PBR/TextureManager
    ${FUNGT_BASE_DIR}/PBR/BVH
    ${FUNGT_BASE_DIR}/vendor/stb_image
    ${FUNGT_BASE_DIR}/Textures
    ${FUNGT_BASE_DIR}/Renders
    ${FUNGT_BASE_DIR}/Matrix
    ${FUNGT_BASE_DIR}/gpu/data
    ${FUNGT_BASE_DIR}/gpu/include
)

# ────────────────────────────────────────────────────────
# CORE PBR LIBRARY (no GPU-specific code)
# ────────────────────────────────────────────────────────
set(PBR_CORE_FILES
    ${FUNGT_BASE_DIR}/PBR/Intersection/intersection.cpp
    ${FUNGT_BASE_DIR}/PBR/BVH/bvh_builder.cpp
    ${FUNGT_BASE_DIR}/PBR/Render/src/compute_backends.cpp
    ${FUNGT_BASE_DIR}/PBR/Render/src/cpu_renderer.cpp
    ${FUNGT_BASE_DIR}/Material/material.cpp
    ${FUNGT_BASE_DIR}/Shaders/shader.cpp
    ${FUNGT_BASE_DIR}/Random/random.cpp
    ${FUNGT_BASE_DIR}/Geometries/cube.cpp
    ${FUNGT_BASE_DIR}/Geometries/plane.cpp
    ${FUNGT_BASE_DIR}/Geometries/primitives.cpp
    ${FUNGT_BASE_DIR}/Geometries/pyramid.cpp
    ${FUNGT_BASE_DIR}/Geometries/shape.cpp
    ${FUNGT_BASE_DIR}/Geometries/inf_grid.cpp
    ${FUNGT_BASE_DIR}/Geometries/square.cpp
    ${FUNGT_BASE_DIR}/Geometries/sphere.cpp
    ${FUNGT_BASE_DIR}/Geometries/box.cpp
    ${FUNGT_BASE_DIR}/SimpleModel/simple_model.cpp
    ${FUNGT_BASE_DIR}/SimpleGeometry/simple_geometry.cpp
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
)

add_library(pbr_core STATIC ${PBR_CORE_FILES})

target_include_directories(pbr_core PUBLIC 
    ${FUNGT_INCLUDES}
    ${FUNLIB_DIR}/include
)

# Compile core with -fsycl (same as main FunGT)
target_compile_options(pbr_core PRIVATE -fsycl)
target_link_options(pbr_core PRIVATE -fsycl)

# Core library compiled with Intel clang (same as SYCL compiler)
message(STATUS "Core PBR library configured (compiler: Intel clang + -fsycl)")

# ────────────────────────────────────────────────────────
# CUDA BACKEND LIBRARY
# ────────────────────────────────────────────────────────
if(FUNGT_USE_CUDA)
    message(STATUS "──────────────────────────────────────")
    message(STATUS "CUDA backend ENABLED")
    message(STATUS "──────────────────────────────────────")
    
    enable_language(CUDA)
    
    set(CUDA_FILES
        ${FUNGT_BASE_DIR}/PBR/Render/src/cuda_renderer.cu
        ${FUNGT_BASE_DIR}/PBR/TextureManager/cuda_texture.cpp
    )
    
    add_library(pbr_cuda STATIC ${CUDA_FILES})
    
    target_include_directories(pbr_cuda PUBLIC 
        ${FUNGT_INCLUDES}
        ${FUNLIB_DIR}/include
    )
    
    # CRITICAL: Mark ALL files as CUDA (not just .cu files!)
    set_source_files_properties(
        ${FUNGT_BASE_DIR}/PBR/Render/src/cuda_renderer.cu
        ${FUNGT_BASE_DIR}/PBR/TextureManager/cuda_texture.cpp
        PROPERTIES LANGUAGE CUDA
    )
    
    set_target_properties(pbr_cuda PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        CUDA_ARCHITECTURES "native"
    )
    
    target_compile_definitions(pbr_cuda PUBLIC 
        FUNGT_USE_CUDA
    )
    
    message(STATUS "CUDA library configured")
    message(STATUS "  Files: cuda_renderer.cu, cuda_texture.cpp")
    message(STATUS "  Compiler: nvcc")
    
else()
    message(STATUS "CUDA backend disabled")
endif()

# ────────────────────────────────────────────────────────
# SYCL BACKEND LIBRARY
# ────────────────────────────────────────────────────────
if(FUNGT_USE_SYCL)
    message(STATUS "──────────────────────────────────────")
    message(STATUS "SYCL backend ENABLED")
    message(STATUS "──────────────────────────────────────")
    
    set(SYCL_FILES
        ${FUNGT_BASE_DIR}/PBR/Render/src/sycl_renderer.cpp
        ${FUNGT_BASE_DIR}/PBR/TextureManager/sycl_texture.cpp
    )
    
    add_library(pbr_sycl STATIC ${SYCL_FILES})
    
    target_include_directories(pbr_sycl PUBLIC 
        ${FUNGT_INCLUDES}
        ${FUNLIB_DIR}/include
    )
    
    # Link GLEW, OpenGL, and glfw so funlib can compile (same as main FunGT)
    target_link_libraries(pbr_sycl PRIVATE 
        OpenGL::GL
        GLEW::GLEW
        glfw
    )
    
    # Add SYCL-specific flags
    target_compile_options(pbr_sycl PRIVATE -fsycl)
    target_link_options(pbr_sycl PRIVATE -fsycl)
    
    target_compile_definitions(pbr_sycl PUBLIC 
        FUNGT_USE_SYCL
    )
    
    message(STATUS "SYCL library configured")
    message(STATUS "  Files: sycl_renderer.cpp, sycl_texture.cpp")
    message(STATUS "  Flags: -fsycl")
    
else()
    message(STATUS "SYCL backend disabled")
endif()

# ────────────────────────────────────────────────────────
# Find required packages
# ────────────────────────────────────────────────────────
find_package(OpenCL REQUIRED)
if(OpenCL_FOUND)
    message(STATUS "OpenCL found: ${OpenCL_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "OpenCL library not found!")
endif()

find_package(OpenGL REQUIRED)
if(OpenGL_FOUND)
    message(STATUS "OpenGL found")
else()
    message(FATAL_ERROR "OpenGL library not found!")
endif()

find_package(glfw3 REQUIRED)
if(glfw3_FOUND)
    message(STATUS "GLFW found")
else()
    message(FATAL_ERROR "GLFW library not found!")
endif()

find_package(GLEW REQUIRED)
if(GLEW_FOUND)
    message(STATUS "GLEW found")
else()
    message(FATAL_ERROR "GLEW library not found!")
endif()

find_package(assimp REQUIRED)
if(assimp_FOUND)
    message(STATUS "Assimp found")
else()
    message(FATAL_ERROR "Assimp library not found!")
endif()

find_package(glm CONFIG REQUIRED)
if(glm_FOUND)
    message(STATUS "GLM found")
else()
    message(FATAL_ERROR "GLM not found!")
endif()

# ────────────────────────────────────────────────────────
# FINAL EXECUTABLE
# ────────────────────────────────────────────────────────
add_executable(pbr_render 
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${FUNGT_BASE_DIR}/PBR/Space/space.cpp  # ← ADD THIS!
)

# CRITICAL: Add -fsycl IMMEDIATELY after add_executable
if(FUNGT_USE_SYCL)
    target_compile_options(pbr_render PRIVATE -fsycl)
    target_link_options(pbr_render PRIVATE -fsycl)
    target_compile_definitions(pbr_render PRIVATE FUNGT_USE_SYCL)
endif()

if(FUNGT_USE_CUDA)
    target_compile_definitions(pbr_render PRIVATE FUNGT_USE_CUDA)
endif()

target_include_directories(pbr_render PRIVATE 
    ${FUNGT_INCLUDES}
    ${OpenCL_INCLUDE_DIRS}
)

# Link core library + conditionally link backend libraries
target_link_libraries(pbr_render PRIVATE 
    pbr_core
    $<$<BOOL:${FUNGT_USE_CUDA}>:pbr_cuda>
    $<$<BOOL:${FUNGT_USE_SYCL}>:pbr_sycl>
    OpenGL::GL
    glfw
    GLEW::GLEW
    assimp::assimp
    glm::glm
    funlib
    dl
    ${OpenCL_LIBRARIES}
)

# ────────────────────────────────────────────────────────
# Build summary
# ────────────────────────────────────────────────────────
message(STATUS "")
message(STATUS "══════════════════════════════════════")
message(STATUS "  PBR Render Build Configuration")
message(STATUS "══════════════════════════════════════")
message(STATUS "Build type:       ${CMAKE_BUILD_TYPE}")
message(STATUS "FunGT base:       ${FUNGT_BASE_DIR}")
message(STATUS "Default compiler: Intel clang (SYCL-capable)")
if(FUNGT_USE_CUDA)
    message(STATUS "CUDA backend:     ENABLED (nvcc)")
else()
    message(STATUS "CUDA backend:     DISABLED")
endif()
if(FUNGT_USE_SYCL)
    message(STATUS "SYCL backend:     ENABLED (-fsycl)")
else()
    message(STATUS "SYCL backend:     DISABLED")
endif()
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "══════════════════════════════════════")
message(STATUS "")