cmake_minimum_required(VERSION 3.7)
project(ked)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_TTF_INCLUDE_DIRS})

add_executable(ked main.c la.c)
target_link_libraries(ked SDL2::SDL2 SDL2_ttf::SDL2_ttf)