cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
project(ked)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules" ${CMAKE_MODULE_PATH})

set(KED_COMPILE_FLAGS "-Wall -Wextra -std=c11 -pedantic -ggdb")

add_definitions(${KED_COMPILE_FLAGS})

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${KED_COMPILE_FLAGS}")

set(SDL2_DIR "vendor/SDL2")
set(SDL2_TTF_DIR "vendor/SDL2_ttf")

add_subdirectory(${SDL2_DIR})
include_directories(${SDL2_DIR}/include)

add_subdirectory(${SDL2_TTF_DIR})
include_directories(${SDL2_TTF_DIR}/include)

add_executable(ked main.c la.c editor.c)
target_link_libraries(ked SDL2main SDL2 SDL2_ttf)
