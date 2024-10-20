cmake_minimum_required(VERSION 3.10)
project(BouncingBallsSimulation)

set(CMAKE_CXX_STANDARD 11)

# Add the source files
add_executable(BouncingBalls main.cpp)

# Include GLFW
include_directories(/opt/homebrew/opt/glfw/include)
find_library(GLFW_LIBRARY glfw PATHS /opt/homebrew/opt/glfw/lib)
target_link_libraries(BouncingBalls ${GLFW_LIBRARY})

# Include OpenGL
find_library(OpenGL_LIBRARY OpenGL)
target_link_libraries(BouncingBalls ${OpenGL_LIBRARY})
