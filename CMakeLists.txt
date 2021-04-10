cmake_minimum_required(VERSION 3.17)
project(rayMarching)

set(CMAKE_CXX_STANDARD 14)

find_package(SDL2 REQUIRED)


add_executable(rayMarching main.cpp objects.cpp objects.h)
target_link_libraries(rayMarching GL SDL2)



add_executable(fractal2D fractal2D.cpp  objects.cpp objects.h)
target_link_libraries(fractal2D GL SDL2)