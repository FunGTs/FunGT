cmake_minimum_required(VERSION 3.14)
project(FunGT_Tests)

# Set FUNGT_BASE_DIR if not already set
if(NOT DEFINED FUNGT_BASE_DIR)
    set(FUNGT_BASE_DIR ${CMAKE_SOURCE_DIR}/..)
endif()

# Download GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Collect source files needed for tests
set(FUNGT_SOURCES
    ${FUNGT_BASE_DIR}/Matrix/matrix3x3f.cpp           # Add Matrix implementation
    # Add other .cpp files as needed
)

# Your test executable
add_executable(fungt_tests
    test_gpu_vec3.cpp
    test_gpu_rigid_body.cpp
    test_integration_kernel.cpp
    ${FUNGT_SOURCES}  # Include source files!
)

# Include directories
target_include_directories(fungt_tests PRIVATE
    ${FUNGT_BASE_DIR}
    ${FUNGT_BASE_DIR}/Vector
    ${FUNGT_BASE_DIR}/Quaternion
    ${FUNGT_BASE_DIR}/Matrix
    ${FUNGT_BASE_DIR}/Physics/GPU
)

target_link_libraries(fungt_tests
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(fungt_tests)