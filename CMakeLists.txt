cmake_minimum_required(VERSION 3.17)
project(WordComplete)

include_directories(include)
add_subdirectory(pybind11)


set(CMAKE_CXX_STANDARD 14)
set(GCC_COVERAGE_COMPILE_FLAGS "-O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

pybind11_add_module(pywordcomplete SHARED NO_EXTRAS python/word_complete.cpp)

#add_executable(WordComplete main.cpp include/huffman/Huffman.h)