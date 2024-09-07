# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name and the programming language
project(MyProject LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the compiler to clang++
set(CMAKE_CXX_COMPILER clang++)

# Set the build type to Debug
#set(CMAKE_BUILD_TYPE Debug)

# Add the executable target
add_executable(MyProject main.cpp png_lib/fpng.cpp geometry/Geometry.cpp sliceAlgo/SliceAlgo.cpp active-object/ActiveObject.cpp stl/loadSTL.cpp
geometry/contourGen.cpp
geometry/isPolygonInside.cpp
geometry/Geometry.h 
stl/loadSTL.h
sliceAlgo/SliceAlgo.h 
geometry/contourGen.h 
geometry/isPolygonInside.h 
png_lib/fpng.h 
active-object/ActiveObject.h
active-object/IMessage.h)

#target_compile_options(MyProject PRIVATE -g -Wall -Wextra -Wpedantic)
target_compile_options(MyProject PRIVATE -g)
target_include_directories(MyProject PRIVATE ${CMAKE_SOURCE_DIR})
# Include directories (if your headers are in different directories, add them here)
# target_include_directories(MyProject PRIVATE ${CMAKE_SOURCE_DIR})