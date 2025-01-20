cmake_minimum_required(VERSION 3.15)

# Project name
project(CORE)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add GLFW (modify path to your built GLFW folder)
add_subdirectory(ext/glfw-3.4)

# glfw includes + other includes
include_directories(ext/glfw-3.4/include ext/include)


set(SOURCES
    src/main.cpp
    src/glad.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

# link glfw
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})