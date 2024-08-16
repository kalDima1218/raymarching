cmake_minimum_required(VERSION 3.23)
project(raymarching)

set(CMAKE_CXX_STANDARD 17)

add_executable(raymarching main.cpp raymarching.h sdf.h
        utility.h lodepng.h lodepng.cpp)
