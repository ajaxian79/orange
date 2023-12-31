cmake_minimum_required(VERSION 3.26)
project(orange C CXX)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

# GLFW
set(GLFW_DIR vendor/glfw)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)
include_directories(${GLFW_DIR}/include)

# Dear ImGui
set(IMGUI_DIR vendor/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

# GLM
set(GLM_DIR vendor/glm)
include_directories(${GLM_DIR} ..)

# stb_image
set(STB_IMAGE_DIR vendor/stb_image)
include_directories(${STB_IMAGE_DIR} ..)

# spdlog

set(SPDLOG_DIR vendor/spdlog)
add_subdirectory(${SPDLOG_DIR})
include_directories(${SPDLOG_DIR}/include ..)

# Libraries
#find_package(Vulkan REQUIRED)
#find_library(VULKAN_LIBRARY
#NAMES vulkan vulkan-1)
#set(LIBRARIES "glfw;${VULKAN_LIBRARY}")
#set(LIBRARIES "glfw;Vulkan::Vulkan")

# Use vulkan headers from glfw:
include_directories(${GLFW_DIR}/deps)

file(GLOB_RECURSE app_sources ./src/*.cpp)
file(GLOB_RECURSE lib_sources ./lib/*.cpp)

add_executable(orange ${app_sources} ${lib_sources} ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp)

if (WIN32)
    find_package(OpenGL REQUIRED)

    if(OPENGL_FOUND)
        message('OPENGL_FOUND-is-true')
    else()
        message('OPENGL_FOUND-is-false')
    endif()

    message(STATUS "OPENGL_INCLUDE_DIR:= ${OPENGL_INCLUDE_DIR}")
    message(STATUS "OPENGL_LIBRARY:= ${OPENGL_LIBRARY}")
    message(STATUS "OPENGL_LIBRARIES:= ${OPENGL_LIBRARIES}")

    set(LIBRARIES "glfw;${OPENGL_LIBRARY}")
else ()
    find_library(OPENGL_LIBRARY NAMES GL OpenGL)
    set(LIBRARIES "glfw;${OPENGL_LIBRARY}")
endif ()

target_link_libraries(orange ${LIBRARIES})
target_compile_definitions(orange PUBLIC -DImTextureID=ImU64)

if(APPLE)
    target_compile_definitions(orange PUBLIC INCLUDE_OPENGL)
else()
    target_compile_definitions(orange PUBLIC INCLUDE_GL)
endif()

if(CMAKE_SYSTEM_NAME MATCHES Darwin)
    target_compile_definitions(orange PUBLIC BUILD_SYSTEM_DARWIN)
elseif (CMAKE_SYSTEM_NAME MATCHES Linux)
    target_compile_definitions(orange PUBLIC BUILD_SYSTEM_LINUX)
elseif (CMAKE_SYSTEM_NAME MATCHES Windows)
    target_compile_definitions(orange PUBLIC BUILD_SYSTEM_WINDOWS)
else ()
    target_compile_definitions(orange PUBLIC BUILD_SYSTEM_UNKNOWN)
endif ()