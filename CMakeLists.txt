cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER /usr/bin/g++-9)

project(particle_sim C CXX)

file(MAKE_DIRECTORY output output/data)

include_directories(src/include/)
include_directories(src/include/imgui/)

find_package(OpenGL REQUIRED)
FIND_PACKAGE ( OpenMP  REQUIRED )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Ofast -std=c++11 -mtune=native -march=native -fopenmp ") #-fsanitize=thread

add_executable(
	plummer_collision src/plummer_collision.cpp 
	src/include/glad.c 
)

target_link_libraries(plummer_collision glfw)
