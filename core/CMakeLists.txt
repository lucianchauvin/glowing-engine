cmake_minimum_required(VERSION 3.15)

# Project name
project(CORE)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add GLFW (modify path to your built GLFW folder)
add_subdirectory(ext/glfw-3.4)

# glfw includes + other includes
include_directories(
    ext/include
    ext/glfw-3.4/include
    ext/dearimgui
)

set(SOURCES
    src/main.cpp
    ext/include/glad/glad.c
    ext/include/dearimgui/imgui.cpp
    ext/include/dearimgui/imgui_draw.cpp
    ext/include/dearimgui/imgui_widgets.cpp
    ext/include/dearimgui/imgui_tables.cpp
    ext/include/dearimgui/imgui_impl_glfw.cpp
    ext/include/dearimgui/imgui_impl_opengl3.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# link glfw
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})