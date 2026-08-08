cmake_minimum_required(VERSION 3.15)
project(FunGT)

# ════════════════════════════════════════════════════════════════════════════
# GPU BACKEND OPTIONS
# ════════════════════════════════════════════════════════════════════════════
option(FUNGT_USE_CUDA "Enable CUDA backend for NVIDIA GPUs" ON)
option(FUNGT_USE_SYCL "Enable SYCL backend for Intel GPUs" ON)
set(SYCL_CUDA_ARCH sm_75 CACHE STRING "NVIDIA GPU architecture for SYCL CUDA backend")
set_property(CACHE SYCL_CUDA_ARCH PROPERTY STRINGS sm_50 sm_52 sm_53 sm_60 sm_61 sm_62 sm_70 sm_72 sm_75 sm_80 sm_86 sm_87 sm_89 sm_90)

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

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# ════════════════════════════════════════════════════════════════════════════
# SYCL TOOLCHAIN DETECTION
# ════════════════════════════════════════════════════════════════════════════
# Priority:
# 1. User-provided SYCL_COMPILER_PATH (cmake -DSYCL_COMPILER_PATH=...)
# 2. Bundled SYCL toolchain in lib/linux_x64/dpcpp/
# 3. Error - user must provide SYCL compiler

if(UNIX)
    if(DEFINED SYCL_COMPILER_PATH)
        # User provided their own SYCL compiler
        if(NOT EXISTS "${SYCL_COMPILER_PATH}")
            message(FATAL_ERROR "SYCL_COMPILER_PATH does not exist: ${SYCL_COMPILER_PATH}")
        endif()
        set(CMAKE_CXX_COMPILER ${SYCL_COMPILER_PATH})
        message(STATUS "Using user-provided SYCL compiler: ${SYCL_COMPILER_PATH}")
    else()
        # Try bundled SYCL toolchain
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
elseif(WIN32)
    if(DEFINED SYCL_COMPILER_PATH)
        # User provided their own SYCL compiler
        if(NOT EXISTS "${SYCL_COMPILER_PATH}")
            message(FATAL_ERROR "SYCL_COMPILER_PATH does not exist: ${SYCL_COMPILER_PATH}")
        endif()
        set(CMAKE_CXX_COMPILER ${SYCL_COMPILER_PATH})
        message(STATUS "Using user-provided SYCL compiler: ${SYCL_COMPILER_PATH}")
    else()
        # Try bundled SYCL toolchain
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

# ════════════════════════════════════════════════════════════════════════════
# PBR RENDERER LIBRARIES - PREBUILT FROM PBR/main/build
# ════════════════════════════════════════════════════════════════════════════
set(PBR_LIB_DIR ${FUNGT_BASE_DIR}/PBR/main/build)

if(UNIX)
    # Set the build type to Release
    set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/release/linux)
    
    # Add the prebuilt ImGui library
    add_library(imgui STATIC IMPORTED)
    set_target_properties(imgui PROPERTIES
        IMPORTED_LOCATION "${FUNGT_BASE_DIR}/vendor/imgui/lib/libimgui.a"
        INTERFACE_INCLUDE_DIRECTORIES "${FUNGT_BASE_DIR}/vendor/imgui"
    )
    
    # Add funlib
    set(FUNLIB_DIR ${FUNGT_BASE_DIR}/vendor/funlib)
    add_library(funlib STATIC IMPORTED GLOBAL)
    set_target_properties(funlib PROPERTIES
        IMPORTED_LOCATION ${FUNLIB_DIR}/lib/libfunlib.a
        INTERFACE_INCLUDE_DIRECTORIES ${FUNLIB_DIR}/include
    )
    # PBR Core (always required)
    add_library(pbr_core STATIC IMPORTED)
    set_target_properties(pbr_core PROPERTIES
        IMPORTED_LOCATION "${PBR_LIB_DIR}/libpbr_core.a"
    )

    # PBR CUDA renderer (if enabled)
    if(FUNGT_USE_CUDA)
        add_library(pbr_cuda STATIC IMPORTED)
        set_target_properties(pbr_cuda PROPERTIES
            IMPORTED_LOCATION "${PBR_LIB_DIR}/libpbr_cuda.a"
        )
        set(PBR_CUDA_LIB pbr_cuda)
        message(STATUS "PBR CUDA renderer library: ${PBR_LIB_DIR}/libpbr_cuda.a")
    endif()

    # PBR SYCL renderer (if enabled)
    if(FUNGT_USE_SYCL)
        add_library(pbr_sycl STATIC IMPORTED)
        set_target_properties(pbr_sycl PROPERTIES
            IMPORTED_LOCATION "${PBR_LIB_DIR}/libpbr_sycl.a"
        )
        set(PBR_SYCL_LIB pbr_sycl)
        message(STATUS "PBR SYCL renderer library: ${PBR_LIB_DIR}/libpbr_sycl.a")
    endif()
endif()

if (WIN32)
    # Add the macro definition
    add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
    # Set vcpkg toolchain file
    set(CMAKE_TOOLCHAIN_FILE "C:/Users/juang/Documents/Development/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    # Set CMake prefix path for vcpkg packages
    set(CMAKE_PREFIX_PATH "C:/Users/juang/Documents/Development/vcpkg/installed/x64-windows")
    set(GLFW_BUILD_STATIC ON)
    message(STATUS "Working Dir: " ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# Include directories
include_directories(
    ${FUNGT_BASE_DIR}
    ${FUNGT_BASE_DIR}/GT
    ${FUNGT_BASE_DIR}/VertexGL
    ${FUNGT_BASE_DIR}/Shaders
    ${FUNGT_BASE_DIR}/funGT
    ${FUNGT_BASE_DIR}/AnimatedModel
    ${FUNGT_BASE_DIR}/Textures
    ${FUNGT_BASE_DIR}/vendor/stb_image
    ${FUNGT_BASE_DIR}/vendor/glm/include
    ${FUNGT_BASE_DIR}/Imgui_Setup
    ${FUNGT_BASE_DIR}/Material
    ${FUNGT_BASE_DIR}/Mesh
    ${FUNGT_BASE_DIR}/Camera
    ${FUNGT_BASE_DIR}/Geometries
    ${FUNGT_BASE_DIR}/Model
    ${FUNGT_BASE_DIR}/Helpers
    ${FUNGT_BASE_DIR}/Animation
    ${FUNGT_BASE_DIR}/Bone
    ${FUNGT_BASE_DIR}/Matrix
    ${FUNGT_BASE_DIR}/SceneManager
    ${FUNGT_BASE_DIR}/CubeMap
    ${FUNGT_BASE_DIR}/ParticleSimulation
    ${FUNGT_BASE_DIR}/Random
    ${FUNGT_BASE_DIR}/Path_Manager
    ${FUNGT_BASE_DIR}/InfoWindow
    ${FUNGT_BASE_DIR}/GUI
    ${FUNGT_BASE_DIR}/GUI/physics
    ${FUNGT_BASE_DIR}/SimpleModel
    ${FUNGT_BASE_DIR}/SimpleGeometry
    ${FUNGT_BASE_DIR}/Physics/CollisionManager
    ${FUNGT_BASE_DIR}/Physics/Contact
    ${FUNGT_BASE_DIR}/Physics/RigidBody
    ${FUNGT_BASE_DIR}/Physics/Shapes
    ${FUNGT_BASE_DIR}/Physics/Integrators
    ${FUNGT_BASE_DIR}/Physics/ContactHelpers
    ${FUNGT_BASE_DIR}/Physics/PhysicsWorld
    ${FUNGT_BASE_DIR}/Physics/AnimationCreator
    ${FUNGT_BASE_DIR}/Physics/Clothing
    ${FUNGT_BASE_DIR}/Quaternion
    ${FUNGT_BASE_DIR}/Vector
    ${FUNGT_BASE_DIR}/ViewPort
    ${FUNGT_BASE_DIR}/Renders
    ${FUNGT_BASE_DIR}/Platform/OpenGL
    ${FUNGT_BASE_DIR}/InfoDevice
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
    ${FUNGT_BASE_DIR}/PBR/TriangleExtractor
    ${FUNGT_BASE_DIR}/PBR/BVH
    ${FUNGT_BASE_DIR}/Triangle
    ${FUNGT_BASE_DIR}/Gizmo/LightGizmo
    ${FUNGT_BASE_DIR}/Lights
    ${FUNGT_BASE_DIR}/GraphicsRenderBackend
    ${FUNGT_BASE_DIR}/GraphicsRenderBackend/opengl
)

# ════════════════════════════════════════════════════════════════════════════
# COMMON SOURCE FILES (compiled with SYCL compiler)
# ════════════════════════════════════════════════════════════════════════════
set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${FUNGT_BASE_DIR}/GT/graphicsTool.cpp
    ${FUNGT_BASE_DIR}/VertexGL/vertexArrayObjects.cpp
    ${FUNGT_BASE_DIR}/VertexGL/vertexBuffers.cpp
    ${FUNGT_BASE_DIR}/VertexGL/vertexIndices.cpp
    ${FUNGT_BASE_DIR}/Shaders/shader.cpp
    ${FUNGT_BASE_DIR}/Shaders/opengl/opengl_shader.cpp
    ${FUNGT_BASE_DIR}/funGT/fungt.cpp
    ${FUNGT_BASE_DIR}/AnimatedModel/animated_model.cpp
    ${FUNGT_BASE_DIR}/Textures/textures.cpp
    ${FUNGT_BASE_DIR}/vendor/stb_image/stb_image.cpp
    ${FUNGT_BASE_DIR}/Imgui_Setup/imgui_setup.cpp
    ${FUNGT_BASE_DIR}/Material/material.cpp
    ${FUNGT_BASE_DIR}/Mesh/mesh.cpp
    ${FUNGT_BASE_DIR}/Camera/camera.cpp
    ${FUNGT_BASE_DIR}/Geometries/cube.cpp
    ${FUNGT_BASE_DIR}/Geometries/plane.cpp
    ${FUNGT_BASE_DIR}/Geometries/primitives.cpp
    ${FUNGT_BASE_DIR}/Geometries/pyramid.cpp
    ${FUNGT_BASE_DIR}/Geometries/shape.cpp
    ${FUNGT_BASE_DIR}/Geometries/inf_grid.cpp
    ${FUNGT_BASE_DIR}/Geometries/square.cpp
    ${FUNGT_BASE_DIR}/Geometries/sphere.cpp
    ${FUNGT_BASE_DIR}/Geometries/box.cpp
    ${FUNGT_BASE_DIR}/Model/model.cpp
    ${FUNGT_BASE_DIR}/Helpers/helpers.cpp
    ${FUNGT_BASE_DIR}/Animation/animation.cpp
    ${FUNGT_BASE_DIR}/Bone/bone.cpp
    ${FUNGT_BASE_DIR}/Matrix/matrix4x4f.cpp
    ${FUNGT_BASE_DIR}/Matrix/matrix3x3f.cpp
    ${FUNGT_BASE_DIR}/SceneManager/scene_manager.cpp
    ${FUNGT_BASE_DIR}/CubeMap/cube_map.cpp
    ${FUNGT_BASE_DIR}/ParticleSimulation/particle_simulation.cpp
    ${FUNGT_BASE_DIR}/ParticleSimulation/particle_simulation_rtc.cpp
    ${FUNGT_BASE_DIR}/ParticleSimulation/particle_simulation_sycl_ops.cpp
    ${FUNGT_BASE_DIR}/ParticleSimulation/particle_rtc_sycl_ops.cpp
    ${FUNGT_BASE_DIR}/Random/random.cpp
    ${FUNGT_BASE_DIR}/Path_Manager/path_manager.cpp
    ${FUNGT_BASE_DIR}/InfoWindow/infowindow.cpp
    ${FUNGT_BASE_DIR}/GUI/gui.cpp
    ${FUNGT_BASE_DIR}/GUI/physics/simulation_controller_window.cpp
    ${FUNGT_BASE_DIR}/GUI/demo_particles_window.cpp
    ${FUNGT_BASE_DIR}/GUI/render_action_window.cpp
    ${FUNGT_BASE_DIR}/GUI/physics/debug_rigidbody_renderer.cpp
    ${FUNGT_BASE_DIR}/SimpleModel/simple_model.cpp
    ${FUNGT_BASE_DIR}/SimpleGeometry/simple_geometry.cpp
    ${FUNGT_BASE_DIR}/Physics/Collisions/simple_collision.cpp
    ${FUNGT_BASE_DIR}/Physics/Collisions/manifold_collision.cpp
    ${FUNGT_BASE_DIR}/Physics/CollisionManager/collision_manager.cpp
    ${FUNGT_BASE_DIR}/Physics/Collider/collider.cpp
    ${FUNGT_BASE_DIR}/Physics/AnimationCreator/key_frame_recorder.cpp
    ${FUNGT_BASE_DIR}/Physics/AnimationCreator/animation_exporter.cpp
    ${FUNGT_BASE_DIR}/ViewPort/viewport.cpp
    ${FUNGT_BASE_DIR}/ViewPort/opengl/opengl_viewport.cpp
    ${FUNGT_BASE_DIR}/ViewPort/opengl/opengl_progressive_path_tracer.cpp
    ${FUNGT_BASE_DIR}/ViewPort/progressive_path_tracer_viewport.cpp
    ${FUNGT_BASE_DIR}/Renders/framebuffer.cpp
    ${FUNGT_BASE_DIR}/Renders/display_graphics.cpp
    ${FUNGT_BASE_DIR}/Platform/OpenGL/OGLframeBuffer.cpp
    ${FUNGT_BASE_DIR}/InfoDevice/gpu_device_info.cpp
    ${FUNGT_BASE_DIR}/PBR/Intersection/intersection.cpp
    ${FUNGT_BASE_DIR}/PBR/Space/space.cpp
    ${FUNGT_BASE_DIR}/PBR/BVH/bvh_builder.cpp
    ${FUNGT_BASE_DIR}/PBR/TriangleExtractor/triangle_extractor.cpp
    ${FUNGT_BASE_DIR}/Gizmo/LightGizmo/light_gizmo_renderer.cpp
    ${FUNGT_BASE_DIR}/GraphicsRenderBackend/graphics_render_device.cpp
    ${FUNGT_BASE_DIR}/GraphicsRenderBackend/gpu_buffer.cpp
    ${FUNGT_BASE_DIR}/GraphicsRenderBackend/gpu_texture.cpp
    ${FUNGT_BASE_DIR}/GraphicsRenderBackend/opengl/opengl_device.cpp
    ${FUNGT_BASE_DIR}/GraphicsRenderBackend/opengl/opengl_buffer.cpp
    ${FUNGT_BASE_DIR}/GraphicsRenderBackend/opengl/opengl_texture.cpp
)

# ════════════════════════════════════════════════════════════════════════════
# GPU DEVICE DETECTION BACKENDS
# ════════════════════════════════════════════════════════════════════════════

# CUDA backend (compiled with g++, NOT SYCL compiler)
if(FUNGT_USE_CUDA)
    find_package(CUDAToolkit REQUIRED)
    
    if(CUDAToolkit_FOUND)
        message(STATUS "CUDA Toolkit found: ${CUDAToolkit_VERSION}")
        
        # Create separate library for CUDA backend
        add_library(cuda_backend STATIC
            ${FUNGT_BASE_DIR}/InfoDevice/cuda_backend.cpp
        )
        
        # CRITICAL: Compile with standard g++, NOT SYCL compiler
        set_source_files_properties(
            ${FUNGT_BASE_DIR}/InfoDevice/cuda_backend.cpp
            PROPERTIES
            COMPILE_FLAGS "-std=c++17"
        )
        
        target_include_directories(cuda_backend PRIVATE
            ${CUDAToolkit_INCLUDE_DIRS}
            ${FUNGT_BASE_DIR}/InfoDevice
        )
        
        target_link_libraries(cuda_backend PRIVATE
            CUDA::cudart
        )
        
        set(CUDA_BACKEND_LIB cuda_backend)
        add_compile_definitions(FUNGT_USE_CUDA)
        message(STATUS "CUDA device detection enabled (compiled with g++)")
        enable_language(CUDA)
    else()
        message(FATAL_ERROR "CUDA requested but CUDA Toolkit not found!")
    endif()
else()
    message(STATUS "CUDA backend disabled")
endif()

# SYCL backend (compiled with SYCL compiler)
if(FUNGT_USE_SYCL)
    list(APPEND SOURCE_FILES ${FUNGT_BASE_DIR}/InfoDevice/sycl_backend.cpp)
    message(STATUS "SYCL device detection enabled (compiled with SYCL compiler)")
else()
    message(STATUS "SYCL backend disabled")
endif()

# ════════════════════════════════════════════════════════════════════════════
# CREATE EXECUTABLE
# ════════════════════════════════════════════════════════════════════════════
add_executable(FunGT ${SOURCE_FILES})
if(FUNGT_USE_CUDA)
    target_compile_definitions(FunGT PRIVATE FUNGT_USE_CUDA)
    target_include_directories(FunGT PRIVATE ${CUDAToolkit_INCLUDE_DIRS})
    set_target_properties(FunGT PROPERTIES
        CUDA_RESOLVE_DEVICE_SYMBOLS ON
        CUDA_SEPARABLE_COMPILATION ON
        CUDA_ARCHITECTURES "native"  
    )
endif()
# ════════════════════════════════════════════════════════════════════════════
# PLATFORM-SPECIFIC CONFIGURATIONS
# ════════════════════════════════════════════════════════════════════════════
if (WIN32)
    # Use vcpkg packages on Windows
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(assimp REQUIRED)
    
    target_link_libraries(FunGT
        PRIVATE
        OpenGL::GL 
        glfw
        GLEW::GLEW
        assimp::assimp
    )
    
elseif (UNIX)
    # ════════════════════════════════════════════════════════════════════════
    # LINUX-SPECIFIC SETTINGS
    # ════════════════════════════════════════════════════════════════════════
    message(STATUS "Configuring for Linux")
    
    # Find required packages
    if(FUNGT_USE_SYCL)

        # Try bundled toolchain OpenCL first
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

    else()

        # If SYCL disabled, just use system OpenCL
        find_package(OpenCL REQUIRED)
        if(OpenCL_FOUND)
            message(STATUS "OpenCL found: ${OpenCL_INCLUDE_DIRS}")
        else()
            message(FATAL_ERROR "OpenCL library not found!")
        endif()

    endif()

    find_package(CURL REQUIRED)
    if (CURL_FOUND)
        message(STATUS "CURL found")
    else()
        message(FATAL_ERROR "CURL library not found!")
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
    
    # Link libraries
    target_include_directories(FunGT PRIVATE ${OpenCL_INCLUDE_DIRS})
    target_link_libraries(FunGT
        PRIVATE
        CURL::libcurl
        OpenGL::GL
        glfw
        GLEW::GLEW
        assimp::assimp
        dl
        funlib
        imgui
        ${OpenCL_LIBRARIES}
        ${CUDA_BACKEND_LIB}
        pbr_core                   
        ${PBR_CUDA_LIB}           
        ${PBR_SYCL_LIB} 
        $<$<BOOL:${FUNGT_USE_CUDA}>:CUDA::cudart_static>
        $<$<BOOL:${FUNGT_USE_CUDA}>:CUDA::cuda_driver>           
    )
    
    # ════════════════════════════════════════════════════════════════════════
    # SYCL compilation flags (per-file, not blanket)
    # ════════════════════════════════════════════════════════════════════════
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

    set(SYCL_SOURCES
        ${FUNGT_BASE_DIR}/InfoDevice/sycl_backend.cpp
        ${FUNGT_BASE_DIR}/ParticleSimulation/particle_simulation_sycl_ops.cpp
        ${FUNGT_BASE_DIR}/ParticleSimulation/particle_rtc_sycl_ops.cpp
        ${FUNGT_BASE_DIR}/PBR/Space/space.cpp
    )

    list(JOIN SYCL_CUDA_ARCH_ARGS " " SYCL_CUDA_ARCH_STR)

    set_source_files_properties(${SYCL_SOURCES}
        PROPERTIES COMPILE_FLAGS "-fsycl -fsycl-targets=${SYCL_TARGETS} ${SYCL_CUDA_ARCH_STR}"
    )

    target_link_options(FunGT PRIVATE -fsycl -fsycl-targets=${SYCL_TARGETS} ${SYCL_CUDA_ARCH_ARGS})
    
    if(FUNGT_USE_SYCL)
        target_compile_definitions(FunGT PRIVATE FUNGT_USE_SYCL)
    endif()
    
endif()

# ════════════════════════════════════════════════════════════════════════════
# BUILD SUMMARY
# ════════════════════════════════════════════════════════════════════════════
message(STATUS "")
message(STATUS "════════════════════════════════════════")
message(STATUS "  FunGT Build Configuration")
message(STATUS "════════════════════════════════════════")
message(STATUS "  Main Compiler:  ${CMAKE_CXX_COMPILER}")
message(STATUS "  CUDA Backend:   ${FUNGT_USE_CUDA}")
message(STATUS "  SYCL Backend:   ${FUNGT_USE_SYCL}")
message(STATUS "  Build Type:     ${CMAKE_BUILD_TYPE}")
if(FUNGT_USE_CUDA)
    message(STATUS "  CUDA Compiled:  Separately with g++")
endif()
message(STATUS "════════════════════════════════════════")
message(STATUS "")