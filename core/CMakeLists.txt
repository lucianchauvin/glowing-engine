# cmake_minimum_required(VERSION 3.15)

# project(CORE)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# # Add GLFW (modify path to your built GLFW folder)
# add_subdirectory(ext/glfw-3.4)
# add_subdirectory(ext/assimp)

# # glfw includes + other includes (headers)
# include_directories(
#     ext
#     ext/glfw-3.4/include
#     ext/assimp/include
#     src
#     src/general
# )

# set(SOURCES
#     src/entity.cpp
#     src/main.cpp
#     src/mesh.cpp
#     src/model_ass.cpp
#     src/model.cpp
#     src/physics.cpp
#     src/scene.cpp

#     ext/glad/glad.c
#     ext/dearimgui/imgui.cpp
#     ext/dearimgui/imgui_draw.cpp
#     ext/dearimgui/imgui_widgets.cpp
#     ext/dearimgui/imgui_tables.cpp
#     ext/dearimgui/imgui_impl_glfw.cpp
#     ext/dearimgui/imgui_impl_opengl3.cpp
# )

# add_executable(${PROJECT_NAME} ${SOURCES})

# target_link_libraries(${PROJECT_NAME} 
#     glfw ${GLFW_LIBRARIES}
#     assimp
# )

cmake_minimum_required(VERSION 3.15)
project(CORE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the path to your local Jolt Physics directory
set(JOLT_PHYSICS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext/JoltPhysics")

# Jolt Physics build options
# When turning this option on, the library will be compiled using doubles for positions. This allows for much bigger worlds.
set(DOUBLE_PRECISION OFF)
# When turning this option on, the library will be compiled with debug symbols
set(GENERATE_DEBUG_SYMBOLS ON)
# When turning this option on, the library will be compiled in such a way to attempt to keep the simulation deterministic across platforms
set(CROSS_PLATFORM_DETERMINISTIC OFF)
# When turning this option on, the library will be compiled with interprocedural optimizations enabled
set(INTERPROCEDURAL_OPTIMIZATION ON)
# When turning this on, the library will emit extra code to ensure that the 4th component of a 3-vector is kept the same as the 3rd component
set(FLOATING_POINT_EXCEPTIONS_ENABLED OFF)
# When turning this on, the library will be compiled with C++ exceptions enabled.
set(CPP_EXCEPTIONS_ENABLED OFF)
# When turning this on, the library will be compiled with C++ RTTI enabled.
set(CPP_RTTI_ENABLED OFF)
# Number of bits to use in ObjectLayer. Can be 16 or 32.
set(OBJECT_LAYER_BITS 16)

# Select X86 processor features to use
set(USE_SSE4_1 ON)
set(USE_SSE4_2 ON)
set(USE_AVX ON)
set(USE_AVX2 ON)
set(USE_AVX512 OFF)
set(USE_LZCNT ON)
set(USE_TZCNT ON)
set(USE_F16C ON)
set(USE_FMADD ON)

# Add GLFW (modify path to your built GLFW folder)
add_subdirectory(ext/glfw-3.4)
add_subdirectory(ext/assimp)

# Add Jolt Physics
add_subdirectory(${JOLT_PHYSICS_DIR}/Build)

# glfw includes + other includes (headers)
include_directories(
  ext
  ext/glfw-3.4/include
  ext/assimp/include
  ${JOLT_PHYSICS_DIR}
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
  ext/glad/glad.c
  ext/dearimgui/imgui.cpp
  ext/dearimgui/imgui_draw.cpp
  ext/dearimgui/imgui_widgets.cpp
  ext/dearimgui/imgui_tables.cpp
  ext/dearimgui/imgui_impl_glfw.cpp
  ext/dearimgui/imgui_impl_opengl3.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
  glfw ${GLFW_LIBRARIES}
  assimp
  Jolt
)