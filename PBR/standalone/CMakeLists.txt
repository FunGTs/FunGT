cmake_minimum_required(VERSION 3.15)

get_filename_component(
    FUNGT_DEFAULT_BASE_DIR
    "${CMAKE_CURRENT_LIST_DIR}/../.."
    ABSOLUTE
)

set(FUNGT_BASE_DIR
    "${FUNGT_DEFAULT_BASE_DIR}"
    CACHE PATH "FunGT base directory"
)

option(FUNGT_USE_CUDA "Link the CUDA PBR backend" ON)
option(FUNGT_USE_SYCL "Link the SYCL PBR backend" ON)
option(FUNGT_USE_OPENCL "Link the OpenCL PBR backend" ON)
set(FUNGT_CUDA_ARCH 89 CACHE STRING "NVIDIA GPU compute capability")
set_property(CACHE FUNGT_CUDA_ARCH PROPERTY STRINGS 50 52 53 60 61 62 70 72 75 80 86 87 89 90)
set(SYCL_CUDA_ARCH "sm_${FUNGT_CUDA_ARCH}")

set(SYCL_COMPILER_PATH
    "/home/juanchuletas/Documents/Development/sycl_workspace/llvm/build/bin/clang++"
    CACHE FILEPATH "SYCL-capable C++ compiler")

if(FUNGT_USE_SYCL)
    if(NOT EXISTS "${SYCL_COMPILER_PATH}")
        message(FATAL_ERROR
            "SYCL compiler not found at ${SYCL_COMPILER_PATH}")
    endif()
    set(CMAKE_CXX_COMPILER
        "${SYCL_COMPILER_PATH}"
        CACHE FILEPATH "C++ compiler" FORCE)
endif()

project(pbr_standalone LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/release/linux")

set(PBR_LIBRARY_DIR
    "${FUNGT_BASE_DIR}/PBR/main/build"
    CACHE PATH "Directory containing the prebuilt PBR static libraries"
)
set(FUNGT_OPENCL_HEADERS
    "$ENV{HOME}/Documents/Development/neo-dev/compute-runtime/third_party/opencl_headers"
    CACHE PATH "Path containing the custom CL headers"
)
set(FUNGT_OPENCL_LOADER
    "/usr/lib/x86_64-linux-gnu/libOpenCL.so.1"
    CACHE FILEPATH "OpenCL ICD loader"
)

function(import_pbr_library target archive)
    if(NOT EXISTS "${PBR_LIBRARY_DIR}/${archive}")
        message(FATAL_ERROR
            "Missing ${archive}. Build the PBR libraries first in ${PBR_LIBRARY_DIR}.")
    endif()

    add_library(${target} STATIC IMPORTED)
    set_target_properties(${target} PROPERTIES
        IMPORTED_LOCATION "${PBR_LIBRARY_DIR}/${archive}"
    )
endfunction()

import_pbr_library(pbr_core libpbr_core.a)

if(FUNGT_USE_CUDA)
    enable_language(CUDA)
    import_pbr_library(pbr_cuda libpbr_cuda.a)
    find_package(CUDAToolkit REQUIRED)
endif()

if(FUNGT_USE_SYCL)
    import_pbr_library(pbr_sycl libpbr_sycl.a)
endif()

if(FUNGT_USE_OPENCL)
    import_pbr_library(pbr_opencl libpbr_opencl.a)
    if(NOT EXISTS "${FUNGT_OPENCL_HEADERS}/CL/cl.h")
        message(FATAL_ERROR
            "Custom OpenCL headers not found at ${FUNGT_OPENCL_HEADERS}")
    endif()
    if(NOT EXISTS "${FUNGT_OPENCL_LOADER}")
        message(FATAL_ERROR
            "OpenCL ICD loader not found at ${FUNGT_OPENCL_LOADER}")
    endif()
endif()

set(FUNLIB_DIR "${FUNGT_BASE_DIR}/vendor/funlib")
add_library(funlib STATIC IMPORTED)
set_target_properties(funlib PROPERTIES
    IMPORTED_LOCATION "${FUNLIB_DIR}/lib/libfunlib.a"
    INTERFACE_INCLUDE_DIRECTORIES "${FUNLIB_DIR}/include"
)

find_package(assimp REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(pbr_standalone main.cpp)

if(FUNGT_USE_CUDA)
    set_target_properties(pbr_standalone PROPERTIES
        CUDA_ARCHITECTURES ${FUNGT_CUDA_ARCH}
        CUDA_SEPARABLE_COMPILATION ON
        CUDA_RESOLVE_DEVICE_SYMBOLS ON
    )
endif()

target_include_directories(pbr_standalone BEFORE PRIVATE
    ${FUNGT_OPENCL_HEADERS}
    ${FUNGT_BASE_DIR}
    ${FUNGT_BASE_DIR}/PBR/Render/include
    ${FUNGT_BASE_DIR}/PBR/TextureManager
    ${FUNGT_BASE_DIR}/vendor/stb_image
    ${FUNGT_BASE_DIR}/vendor/glm/include
    ${FUNLIB_DIR}/include
)

target_compile_definitions(pbr_standalone PRIVATE
    $<$<BOOL:${FUNGT_USE_CUDA}>:FUNGT_USE_CUDA>
    $<$<BOOL:${FUNGT_USE_SYCL}>:FUNGT_USE_SYCL>
    $<$<BOOL:${FUNGT_USE_OPENCL}>:FUNGT_USE_OPENCL>
    $<$<BOOL:${FUNGT_USE_OPENCL}>:CL_TARGET_OPENCL_VERSION=300>
)

if(FUNGT_USE_CUDA)
    target_include_directories(pbr_standalone PRIVATE
        ${CUDAToolkit_INCLUDE_DIRS}
    )
endif()

if(FUNGT_USE_SYCL)
    target_compile_options(pbr_standalone PRIVATE -fsycl)
    if(FUNGT_USE_CUDA)
        set(SYCL_TARGETS nvptx64-nvidia-cuda,spir64)
        set(SYCL_CUDA_ARCH_ARGS
            "-Xsycl-target-backend=nvptx64-nvidia-cuda"
            "--offload-arch=${SYCL_CUDA_ARCH}"
        )
    else()
        set(SYCL_TARGETS spir64)
        set(SYCL_CUDA_ARCH_ARGS "")
    endif()
    target_link_options(pbr_standalone PRIVATE
        -fsycl
        -fsycl-targets=${SYCL_TARGETS}
        ${SYCL_CUDA_ARCH_ARGS}
    )
endif()

target_link_libraries(pbr_standalone PRIVATE
    pbr_core
    $<$<BOOL:${FUNGT_USE_CUDA}>:pbr_cuda>
    $<$<BOOL:${FUNGT_USE_SYCL}>:pbr_sycl>
    $<$<BOOL:${FUNGT_USE_OPENCL}>:pbr_opencl>
    OpenGL::GL
    assimp::assimp
    funlib
    dl
    $<$<BOOL:${FUNGT_USE_OPENCL}>:${FUNGT_OPENCL_LOADER}>
    $<$<BOOL:${FUNGT_USE_CUDA}>:CUDA::cudart_static>
    $<$<BOOL:${FUNGT_USE_CUDA}>:CUDA::cuda_driver>
)

message(STATUS "PBR libraries: ${PBR_LIBRARY_DIR}")
message(STATUS "CUDA backend:   ${FUNGT_USE_CUDA}")
message(STATUS "SYCL backend:   ${FUNGT_USE_SYCL}")
message(STATUS "OpenCL backend: ${FUNGT_USE_OPENCL}")
