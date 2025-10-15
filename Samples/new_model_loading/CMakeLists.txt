cmake_minimum_required(VERSION 3.15)
project(FunGT)

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
if(UNIX)
    # Set the build type to Release
    set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/release/linux)
    set(CMAKE_CXX_COMPILER /home/juanchuletas/Documents/Development/sycl_workspace/llvm/build/bin/clang++)
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
    #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    message(STATUS "Working Dir: " ${CMAKE_CURRENT_SOURCE_DIR})
endif()
# Include directories
include_directories(
    ${FUNGT_BASE_DIR}/VertexGL
    ${FUNGT_BASE_DIR}/Shaders
    ${FUNGT_BASE_DIR}/funGT
    ${FUNGT_BASE_DIR}/AnimatedModel
    ${FUNGT_BASE_DIR}/Textures
    ${FUNGT_BASE_DIR}/vendor/stb_image
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
    ${FUNGT_BASE_DIR}/SimpleModel
    ${FUNGT_BASE_DIR}/Physics/CollisionManager
    ${FUNGT_BASE_DIR}/Physics/Contact
    ${FUNGT_BASE_DIR}/Physics/RigidBody
    ${FUNGT_BASE_DIR}/Physics/Shapes
    ${FUNGT_BASE_DIR}/Physics/Integrators
    ${FUNGT_BASE_DIR}/Physics/ContactHelpers
    ${FUNGT_BASE_DIR}/Physics/PhysicsWorld
    ${FUNGT_BASE_DIR}/Physics/Clothing
    ${FUNGT_BASE_DIR}/Quaternion
    ${FUNGT_BASE_DIR}/Vector
    ${FUNGT_BASE_DIR}/Renders
    

)
# Source files
set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${FUNGT_BASE_DIR}/VertexGL/vertexArrayObjects.cpp
    ${FUNGT_BASE_DIR}/VertexGL/vertexBuffers.cpp
    ${FUNGT_BASE_DIR}/VertexGL/vertexIndices.cpp
    ${FUNGT_BASE_DIR}/Shaders/shader.cpp
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
    ${FUNGT_BASE_DIR}/Geometries/square.cpp
    ${FUNGT_BASE_DIR}/Model/model.cpp
    ${FUNGT_BASE_DIR}/Helpers/helpers.cpp
    ${FUNGT_BASE_DIR}/Animation/animation.cpp
    ${FUNGT_BASE_DIR}/Bone/bone.cpp
    ${FUNGT_BASE_DIR}/Matrix/matrix4x4f.cpp
    ${FUNGT_BASE_DIR}/Matrix/matrix3x3f.cpp
    ${FUNGT_BASE_DIR}/SceneManager/scene_manager.cpp
    ${FUNGT_BASE_DIR}/CubeMap/cube_map.cpp
    #${FUNGT_BASE_DIR}/Physics/ParticleSystem/particle_sys.cpp
    #${FUNGT_BASE_DIR}/Physics/ParticleSystem/particle.cpp
    ${FUNGT_BASE_DIR}/ParticleSimulation/particle_simulation.cpp
    ${FUNGT_BASE_DIR}/Random/random.cpp
    ${FUNGT_BASE_DIR}/Path_Manager/path_manager.cpp
    ${FUNGT_BASE_DIR}/InfoWindow/infowindow.cpp
    ${FUNGT_BASE_DIR}/GUI/gui.cpp
    ${FUNGT_BASE_DIR}/SimpleModel/simple_model.cpp
    ${FUNGT_BASE_DIR}/Physics/Collisions/simple_collision.cpp
    ${FUNGT_BASE_DIR}/Physics/Collider/collider.cpp
    ${FUNGT_BASE_DIR}/Physics/Clothing/clothing.cpp
    ${FUNGT_BASE_DIR}/Renders/display_graphics.cpp
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
    target_include_directories(FunGT PRIVATE ${OpenCL_INCLUDE_DIRS})
    target_link_libraries(FunGT
        PRIVATE
        OpenGL::GL
        glfw
        GLEW::GLEW
        assimp::assimp
	    glm::glm
        dl
        funlib
        imgui
        ${OpenCL_LIBRARIES}
    )
    target_compile_options(FunGT PRIVATE -fsycl)
    target_link_options(FunGT PRIVATE -fsycl)
endif()
