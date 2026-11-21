cmake_minimum_required(VERSION 3.15)
project(blelloch_scan_test)

option(FUNGT_USE_SYCL "Enable SYCL backend" ON)
set(FUNGT_CUDA_ARCH 89 CACHE STRING "NVIDIA GPU compute capability")
set(SYCL_CUDA_ARCH "sm_${FUNGT_CUDA_ARCH}")

if(NOT DEFINED FUNGT_BASE_DIR)
    if(DEFINED ENV{FUNGT_BASE_DIR})
        set(FUNGT_BASE_DIR $ENV{FUNGT_BASE_DIR} CACHE PATH "FunGT base directory")
    else()
        message(FATAL_ERROR "FUNGT_BASE_DIR not specified")
    endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX)
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
endif()

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(FUNLIB_DIR ${FUNGT_BASE_DIR}/vendor/funlib)
add_library(funlib STATIC IMPORTED GLOBAL)
set_target_properties(funlib PROPERTIES
    IMPORTED_LOCATION ${FUNLIB_DIR}/lib/libfunlib.a
    INTERFACE_INCLUDE_DIRECTORIES ${FUNLIB_DIR}/include
)

include_directories(
    ${FUNGT_BASE_DIR}
    ${FUNGT_BASE_DIR}/MarchingCubes
    ${FUNGT_BASE_DIR}/vendor/glm/include
)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${FUNGT_BASE_DIR}/MarchingCubes/gpu/mc_gpu.cpp
)

add_executable(blelloch_scan_test ${SOURCE_FILES})

find_package(CUDAToolkit)
if(CUDAToolkit_FOUND)
    set(SYCL_TARGETS nvptx64-nvidia-cuda,spir64)
    set(SYCL_CUDA_ARCH_ARGS
        "-Xsycl-target-backend=nvptx64-nvidia-cuda"
        "--offload-arch=${SYCL_CUDA_ARCH}"
    )
else()
    set(SYCL_TARGETS spir64)
    set(SYCL_CUDA_ARCH_ARGS "")
endif()

list(JOIN SYCL_CUDA_ARCH_ARGS " " SYCL_CUDA_ARCH_STR)

set_source_files_properties(${SOURCE_FILES}
    PROPERTIES COMPILE_FLAGS "-fsycl -fsycl-targets=${SYCL_TARGETS} ${SYCL_CUDA_ARCH_STR}"
)

set(BUNDLED_OPENCL_INCLUDE "${BUNDLED_SYCL_DIR}/include")
find_library(BUNDLED_OPENCL_LIBRARY
    NAMES OpenCL
    PATHS "${BUNDLED_SYCL_DIR}/lib"
    NO_DEFAULT_PATH
)

if(EXISTS "${BUNDLED_OPENCL_INCLUDE}/CL/opencl.h" AND BUNDLED_OPENCL_LIBRARY)
    set(OpenCL_INCLUDE_DIRS "${BUNDLED_OPENCL_INCLUDE}")
    set(OpenCL_LIBRARIES "${BUNDLED_OPENCL_LIBRARY}")
else()
    find_package(OpenCL REQUIRED)
endif()

find_package(OpenGL REQUIRED)

target_include_directories(blelloch_scan_test PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(blelloch_scan_test
    PRIVATE
    funlib
    OpenGL::GL
    ${OpenCL_LIBRARIES}
    dl
)

target_link_options(blelloch_scan_test PRIVATE -fsycl -fsycl-targets=${SYCL_TARGETS} ${SYCL_CUDA_ARCH_ARGS})
target_compile_definitions(blelloch_scan_test PRIVATE FUNGT_USE_SYCL)

message(STATUS "")
message(STATUS "Blelloch Scan Test Configuration")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  SYCL Targets: ${SYCL_TARGETS}")
message(STATUS "")
