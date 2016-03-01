cmake_minimum_required(VERSION 3.3)
project(BDIF_A)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp buff_parser.h MPI_utility.h test.cpp test.h)
add_executable(BDIF_A ${SOURCE_FILES})