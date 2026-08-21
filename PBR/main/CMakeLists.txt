cmake_minimum_required(VERSION 3.15)
project(pbr_libraries)

option(FUNGT_USE_CUDA "Enable CUDA backend" ON)
option(FUNGT_USE_SYCL "Enable SYCL backend" ON)
option(FUNGT_USE_OPENCL "Enable OpenCL backend" ON)
set(FUNGT_CUDA_ARCH 89 CACHE STRING "NVIDIA GPU compute capability")
set_property(CACHE FUNGT_CUDA_ARCH PROPERTY STRINGS 50 52 53 60 61 62 70 72 75 80 86 87 89 90)
set(SYCL_CUDA_ARCH "sm_${FUNGT_CUDA_ARCH}")

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
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ════════════════════════════════════════════════════════════════════════════
# SYCL TOOLCHAIN DETECTION
# ════════════════════════════════════════════════════════════════════════════
if(FUNGT_USE_SYCL AND UNIX)
    if(DEFINED SYCL_COMPILER_PATH)
        if(NOT EXISTS "${SYCL_COMPILER_PATH}")
            message(FATAL_ERROR "SYCL_COMPILER_PATH does not exist: ${SYCL_COMPILER_PATH}")
        endif()
        set(CMAKE_CXX_COMPILER ${SYCL_COMPILER_PATH})
        message(STATUS "Using user-provided SYCL compiler: ${SYCL_COMPILER_PATH}")
    else()
        set(BUNDLED_SYCL_DIR "${FUNGT_BASE_DIR}/toolchain/sycl/linux_x64/dpcpp")
        set(BUNDLED_SYCL_COMPILER "${BUNDLED_SYCL_DIR}/bin/clang++")

        if(EXISTS "${BUNDLED_SYCL_COMPILER}")
            set(CMAKE_CXX_COMPILER ${BUNDLED_SYCL_COMPILER})
            message(STATUS "Using bundled SYCL compiler: ${BUNDLED_SYCL_COMPILER}")
        else()
            message(FATAL_ERROR
                "SYCL compiler not found!\n"
                "Please either:\n"
                "  1. Use bundled toolchain (should be at ${BUNDLED_SYCL_DIR})\n"
                "  2. Provide your own: cmake -DSYCL_COMPILER_PATH=/path/to/clang++ ..\n")
        endif()
    endif()
elseif(FUNGT_USE_SYCL AND WIN32)
    if(DEFINED SYCL_COMPILER_PATH)
        if(NOT EXISTS "${SYCL_COMPILER_PATH}")
            message(FATAL_ERROR "SYCL_COMPILER_PATH does not exist: ${SYCL_COMPILER_PATH}")
        endif()
        set(CMAKE_CXX_COMPILER ${SYCL_COMPILER_PATH})
        message(STATUS "Using user-provided SYCL compiler: ${SYCL_COMPILER_PATH}")
    else()
        set(BUNDLED_SYCL_DIR "${FUNGT_BASE_DIR}/lib/windows_x64/dpcpp")
        set(BUNDLED_SYCL_COMPILER "${BUNDLED_SYCL_DIR}/bin/clang++.exe")

        if(EXISTS "${BUNDLED_SYCL_COMPILER}")
            set(CMAKE_CXX_COMPILER ${BUNDLED_SYCL_COMPILER})
            message(STATUS "Using bundled SYCL compiler: ${BUNDLED_SYCL_COMPILER}")
        else()
            message(FATAL_ERROR
                "SYCL compiler not found!\n"
                "Please either:\n"
                "  1. Use bundled toolchain (should be at ${BUNDLED_SYCL_DIR})\n"
                "  2. Provide your own: cmake -DSYCL_COMPILER_PATH=C:/path/to/clang++.exe ..\n")
        endif()
    endif()
endif()

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
    ${FUNGT_BASE_DIR}/vendor/glm/include
    ${FUNGT_BASE_DIR}/Textures
    ${FUNGT_BASE_DIR}/Renders
    ${FUNGT_BASE_DIR}/Matrix
    ${FUNGT_BASE_DIR}/Lights
    ${FUNGT_BASE_DIR}/gpu/data
    ${FUNGT_BASE_DIR}/gpu/include
    ${FUNGT_BASE_DIR}/GraphicsRenderBackend
    ${FUNGT_BASE_DIR}/GraphicsRenderBackend/opengl
)

# ────────────────────────────────────────────────────────
# CORE PBR LIBRARY (no GPU-specific code)
# ────────────────────────────────────────────────────────
set(PBR_CORE_FILES
    ${FUNGT_BASE_DIR}/PBR/Space/space.cpp
    ${FUNGT_BASE_DIR}/PBR/Intersection/intersection.cpp
    ${FUNGT_BASE_DIR}/PBR/BVH/bvh_builder.cpp
    ${FUNGT_BASE_DIR}/PBR/Render/src/compute_backends.cpp
    ${FUNGT_BASE_DIR}/PBR/Render/src/cpu_renderer.cpp
    ${FUNGT_BASE_DIR}/Material/material.cpp
    ${FUNGT_BASE_DIR}/Random/random.cpp
    ${FUNGT_BASE_DIR}/Geometries/cube.cpp
    ${FUNGT_BASE_DIR}/Geometries/plane.cpp
    ${FUNGT_BASE_DIR}/Geometries/primitives.cpp
    ${FUNGT_BASE_DIR}/Geometries/pyramid.cpp
    ${FUNGT_BASE_DIR}/Geometries/shape.cpp
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
    ${FUNGT_BASE_DIR}/Matrix/matrix4x4f.cpp
    ${FUNGT_BASE_DIR}/Matrix/matrix3x3f.cpp
    ${FUNGT_BASE_DIR}/Renders/display_graphics.cpp
)

add_library(pbr_core STATIC ${PBR_CORE_FILES})

target_include_directories(pbr_core PUBLIC
    ${FUNGT_INCLUDES}
    ${FUNLIB_DIR}/include
)

message(STATUS "Core PBR library configured (compiler: Intel clang)")

# ────────────────────────────────────────────────────────
# CUDA BACKEND LIBRARY
# ────────────────────────────────────────────────────────
if(FUNGT_USE_CUDA)
    message(STATUS "──────────────────────────────────────")
    message(STATUS "CUDA backend ENABLED")
    message(STATUS "──────────────────────────────────────")

    enable_language(CUDA)
    find_package(CUDAToolkit REQUIRED)

    set(CUDA_FILES
        ${FUNGT_BASE_DIR}/PBR/Render/src/cuda_renderer.cu
        ${FUNGT_BASE_DIR}/PBR/TextureManager/cuda_texture.cpp
    )

    add_library(pbr_cuda STATIC ${CUDA_FILES})

    target_include_directories(pbr_cuda PUBLIC
        ${FUNGT_INCLUDES}
        ${FUNLIB_DIR}/include
        ${CUDAToolkit_INCLUDE_DIRS}
    )

    set_source_files_properties(
        ${FUNGT_BASE_DIR}/PBR/Render/src/cuda_renderer.cu
        ${FUNGT_BASE_DIR}/PBR/TextureManager/cuda_texture.cpp
        PROPERTIES LANGUAGE CUDA
    )

    set_target_properties(pbr_cuda PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        CUDA_ARCHITECTURES ${FUNGT_CUDA_ARCH}
    )

    target_compile_definitions(pbr_cuda PUBLIC FUNGT_USE_CUDA)
    target_compile_definitions(pbr_core PUBLIC FUNGT_USE_CUDA)
    target_include_directories(pbr_core PRIVATE ${CUDAToolkit_INCLUDE_DIRS})

    message(STATUS "CUDA library configured")
    message(STATUS "  Files: cuda_renderer.cu, cuda_texture.cpp")
    message(STATUS "  Compiler: nvcc")
    message(STATUS "  CUDA includes: ${CUDAToolkit_INCLUDE_DIRS}")

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
    if(CUDAToolkit_FOUND)
        message(STATUS "CUDA SYCL backend enabled")
        set(SYCL_TARGETS nvptx64-nvidia-cuda,spir64)
        set(SYCL_CUDA_ARCH_ARGS
            "-Xsycl-target-backend=nvptx64-nvidia-cuda"
            "--offload-arch=${SYCL_CUDA_ARCH}"
        )
    else()
        set(SYCL_TARGETS spir64)
        set(SYCL_CUDA_ARCH_ARGS "")
    endif()
    set(SYCL_FILES
        ${FUNGT_BASE_DIR}/PBR/Render/src/sycl_renderer.cpp
        ${FUNGT_BASE_DIR}/PBR/TextureManager/sycl_texture.cpp
    )

    add_library(pbr_sycl STATIC ${SYCL_FILES})

    target_include_directories(pbr_sycl PUBLIC
        ${FUNGT_INCLUDES}
        ${FUNLIB_DIR}/include
    )

    target_compile_options(pbr_sycl PRIVATE -fsycl -fsycl-targets=${SYCL_TARGETS} ${SYCL_CUDA_ARCH_ARGS})
    target_link_options(pbr_sycl PRIVATE -fsycl -fsycl-targets=${SYCL_TARGETS} ${SYCL_CUDA_ARCH_ARGS})

    target_compile_definitions(pbr_sycl PUBLIC FUNGT_USE_SYCL)
    target_compile_definitions(pbr_core PUBLIC FUNGT_USE_SYCL)
    target_compile_options(pbr_core PRIVATE -fsycl)

    message(STATUS "SYCL library configured")
    message(STATUS "  Files: sycl_renderer.cpp, sycl_texture.cpp")
    message(STATUS "  Flags: -fsycl -fsycl-targets=${SYCL_TARGETS} ${SYCL_CUDA_ARCH_ARGS}")

else()
    message(STATUS "SYCL backend disabled")
endif()

# ────────────────────────────────────────────────────────
# Find required packages
# ────────────────────────────────────────────────────────
if(FUNGT_USE_OPENCL)
    set(FUNGT_OPENCL_HEADERS
        "$ENV{HOME}/Documents/Development/neo-dev/compute-runtime/third_party/opencl_headers"
        CACHE PATH "Path containing the custom CL headers")
    set(FUNGT_OPENCL_LOADER
        "/usr/lib/x86_64-linux-gnu/libOpenCL.so.1"
        CACHE FILEPATH "OpenCL ICD loader used by the standalone PBR test")

    if(NOT EXISTS "${FUNGT_OPENCL_HEADERS}/CL/cl.h")
        message(FATAL_ERROR
            "Custom OpenCL headers not found at: ${FUNGT_OPENCL_HEADERS}")
    endif()
    if(NOT EXISTS "${FUNGT_OPENCL_LOADER}")
        message(FATAL_ERROR
            "OpenCL ICD loader not found at: ${FUNGT_OPENCL_LOADER}")
    endif()

    set(OpenCL_INCLUDE_DIRS "${FUNGT_OPENCL_HEADERS}")
    set(OpenCL_LIBRARIES "${FUNGT_OPENCL_LOADER}")
    message(STATUS "OpenCL backend uses custom compute-runtime headers:")
    message(STATUS "  Include: ${OpenCL_INCLUDE_DIRS}")
    message(STATUS "  Loader:  ${OpenCL_LIBRARIES}")
elseif(FUNGT_USE_SYCL)
    set(BUNDLED_OPENCL_INCLUDE "${BUNDLED_SYCL_DIR}/include")
    find_library(BUNDLED_OPENCL_LIBRARY
        NAMES OpenCL
        PATHS "${BUNDLED_SYCL_DIR}/lib"
        NO_DEFAULT_PATH
    )

    if(EXISTS "${BUNDLED_OPENCL_INCLUDE}/CL/opencl.h" AND BUNDLED_OPENCL_LIBRARY)
        set(OpenCL_INCLUDE_DIRS "${BUNDLED_OPENCL_INCLUDE}")
        set(OpenCL_LIBRARIES "${BUNDLED_OPENCL_LIBRARY}")
        message(STATUS "OpenCL (bundled) found:")
        message(STATUS "  Include: ${OpenCL_INCLUDE_DIRS}")
        message(STATUS "  Library: ${OpenCL_LIBRARIES}")
    else()
        message(STATUS "Bundled OpenCL not found, trying system OpenCL...")
        find_package(OpenCL REQUIRED)
        if(OpenCL_FOUND)
            message(STATUS "OpenCL (system) found: ${OpenCL_INCLUDE_DIRS}")
        else()
            message(FATAL_ERROR "OpenCL library not found!")
        endif()
    endif()
endif()

# ────────────────────────────────────────────────────────
# OPENCL BACKEND LIBRARY
# ────────────────────────────────────────────────────────
if(FUNGT_USE_OPENCL)
    set(OPENCL_FILES
        ${FUNGT_BASE_DIR}/PBR/Render/src/opencl_renderer.cpp
        ${FUNGT_BASE_DIR}/PBR/TextureManager/opencl_texture.cpp
    )

    add_library(pbr_opencl STATIC ${OPENCL_FILES})

    target_include_directories(pbr_opencl BEFORE PUBLIC
        ${FUNGT_OPENCL_HEADERS}
        ${FUNGT_INCLUDES}
        ${FUNLIB_DIR}/include
    )

    target_compile_definitions(pbr_opencl PUBLIC
        FUNGT_USE_OPENCL
        CL_TARGET_OPENCL_VERSION=300
    )
    target_compile_definitions(pbr_core PUBLIC
        FUNGT_USE_OPENCL
        CL_TARGET_OPENCL_VERSION=300
    )
    target_include_directories(pbr_core BEFORE PRIVATE
        ${FUNGT_OPENCL_HEADERS}
    )

    target_link_libraries(pbr_opencl PUBLIC ${FUNGT_OPENCL_LOADER})

    message(STATUS "OpenCL library configured")
    message(STATUS "  Files: opencl_renderer.cpp, opencl_texture.cpp")
endif()

find_package(assimp REQUIRED)
if(assimp_FOUND)
    message(STATUS "Assimp found")
else()
    message(FATAL_ERROR "Assimp library not found!")
endif()

find_package(OpenGL REQUIRED)
if(OpenGL_FOUND)
    message(STATUS "OpenGL found (for GL/SYCL interop)")
else()
    message(FATAL_ERROR "OpenGL library not found!")
endif()

# ────────────────────────────────────────────────────────
# Build summary
# ────────────────────────────────────────────────────────
message(STATUS "")
message(STATUS "══════════════════════════════════════")
message(STATUS "  PBR Library Build Configuration")
message(STATUS "══════════════════════════════════════")
message(STATUS "Build type:       ${CMAKE_BUILD_TYPE}")
message(STATUS "FunGT base:       ${FUNGT_BASE_DIR}")
message(STATUS "Default compiler: Bundled Intel clang (SYCL-capable)")
if(FUNGT_USE_CUDA)
    message(STATUS "CUDA backend:     ENABLED (nvcc)")
    message(STATUS "CUDA includes:    ${CUDAToolkit_INCLUDE_DIRS}")
    message(STATUS "CUDA architecture: ${FUNGT_CUDA_ARCH}")
else()
    message(STATUS "CUDA backend:     DISABLED")
endif()
if(FUNGT_USE_SYCL)
    message(STATUS "SYCL backend:     ENABLED (-fsycl -fsycl-targets=${SYCL_TARGETS})")
else()
    message(STATUS "SYCL backend:     DISABLED")
endif()
if(FUNGT_USE_OPENCL)
    message(STATUS "OpenCL backend:   ENABLED")
    message(STATUS "OpenCL headers:   ${FUNGT_OPENCL_HEADERS}")
    message(STATUS "OpenCL loader:    ${FUNGT_OPENCL_LOADER}")
else()
    message(STATUS "OpenCL backend:   DISABLED")
endif()
message(STATUS "══════════════════════════════════════")
message(STATUS "")
