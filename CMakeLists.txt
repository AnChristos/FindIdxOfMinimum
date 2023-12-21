cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(findIndexOfMinimum-SIMD VERSION 1.0)



# C++ setting
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-Wall -Wextra -pedantic -Werror  -O2 -g -march=x86-64-v2)

list(APPEND CMAKE_PREFIX_PATH $ENV{HOME}/.local/)
find_package(benchmark REQUIRED)
find_package (Threads)

add_executable(findIndexOfMinimum_test 
	       findIndexOfMinimum_test.cxx)

add_executable(findIndexOfMinimum_bench 
	       findIndexOfMinimum_bench.cxx)

target_link_libraries(findIndexOfMinimum_bench  benchmark::benchmark)

