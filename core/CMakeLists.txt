cmake_minimum_required(VERSION 3.15)

project(CORE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add GLFW (modify path to your built GLFW folder)
add_subdirectory(ext/glfw-3.4)
add_subdirectory(ext/assimp)

# glfw includes + other includes (headers)
include_directories(
    ext
    ext/glfw-3.4/include
    ext/assimp/include
    ext/fmodstudioapi20307linux/api/core/inc
    src
    src/general
)

set(SOURCES
    src/entity.cpp
    src/main.cpp
    src/mesh.cpp
    src/model_ass.cpp
    src/model.cpp
    src/physics.cpp
    src/scene.cpp
    src/audio.cpp
    src/renderer_debug.cpp
    src/material_disney.cpp

    ext/glad/glad.c
    ext/dearimgui/imgui.cpp
    ext/dearimgui/imgui_draw.cpp
    ext/dearimgui/imgui_widgets.cpp
    ext/dearimgui/imgui_tables.cpp
    ext/dearimgui/imgui_impl_glfw.cpp
    ext/dearimgui/imgui_impl_opengl3.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

find_library(FMOD_LIBRARY 
    NAMES fmod
    PATHS ${CMAKE_SOURCE_DIR}/ext/fmodstudioapi20307linux/api/core/lib/x86_64
    NO_DEFAULT_PATH
)

target_link_libraries(${PROJECT_NAME} 
    glfw ${GLFW_LIBRARIES}
    assimp
    ${FMOD_LIBRARY}
)
