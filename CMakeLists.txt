
cmake_minimum_required(VERSION 3.8)

project(sc2-bot)

# Specify output directories.
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

# Build with c++14 support, required by sc2api.
set(CMAKE_CXX_STANDARD 20)

# Disable building of examples in the sc2api submodule.
set(BUILD_API_EXAMPLES OFF CACHE INTERNAL "" FORCE)

# Disable building of tests in the sc2api submodule.
set(BUILD_API_TESTS OFF CACHE INTERNAL "" FORCE)

#WORKAROUND: visual studio 2019 16.10 constexpr containers fails to work
#because of debug crt checks
add_definitions(-D_ITERATOR_DEBUG_LEVEL=0 -D_CONTAINTER_DEBUG_LEVEL=0)

add_subdirectory("external/s2client-api")
add_subdirectory("external/glm")
add_subdirectory("src")
