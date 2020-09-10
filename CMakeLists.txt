cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(findMinumumIndex-SIMD VERSION 1.0)



# C++ setting
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-Wall -Wextra -pedantic -Werror  -O2 -g)

list(APPEND CMAKE_PREFIX_PATH $ENV{HOME}/.local/)
find_package(benchmark REQUIRED)
find_package (Threads)

add_executable(findMinumumIndex_test 
	       findMinumumIndex_test.cxx)

add_executable(findMinumumIndex_bench 
	       findMinumumIndex_bench.cxx)

target_link_libraries(findMinumumIndex_bench  benchmark::benchmark)
target_link_libraries (findMinumumIndex_bench  benchmark::benchmark)

