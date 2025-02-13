cmake_minimum_required(VERSION 3.15)
project(FunGT)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
)

# Add executable
add_executable(FunGT ${SOURCE_FILES})

# Platform-specific configurations
if (WIN32)
    # Windows-specific settings
    find_library(OPENGL_LIB NAMES opengl32)
    if (NOT OPENGL_LIB)
        message(FATAL_ERROR "OpenGL library not found!")
    endif()

    find_library(GLFW_LIB NAMES glfw3)
    if (NOT GLFW_LIB)
        message(FATAL_ERROR "GLFW library not found!")
    endif()

    find_library(GLEW_LIB NAMES glew32)
    if (NOT GLEW_LIB)
        message(FATAL_ERROR "GLEW library not found!")
    endif()

    find_library(ASSIMP_LIB NAMES assimp)
    if (NOT ASSIMP_LIB)
        message(FATAL_ERROR "Assimp library not found!")
    endif()

    target_link_libraries(FunGT
        PRIVATE
        ${OPENGL_LIB}
        ${GLFW_LIB}
        ${GLEW_LIB}
        ${ASSIMP_LIB}
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