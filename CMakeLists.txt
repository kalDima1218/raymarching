cmake_minimum_required(VERSION 3.23)
project(raymarching)

set(CMAKE_CXX_STANDARD 17)

add_executable(raymarching main.cpp Scene.h SDF.h
        Vector.h lodepng.h lodepng.cpp
        Vector.cpp
        SDF.cpp
        Scene.cpp)
