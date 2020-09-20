# FindMinimumIndex
Playground for using SIMD for calculating the Index of the minimum in a array of floats.

# If you have already google benchmark installed

``git clone https://github.com/AnChristos/FindMinimumIndex.git``

# Also install google benchmark 

Assuming you want also the google bench mark code

Assuming you want also the google bench mark code

``git clone --recursive https://github.com/AnChristos/FindMinimumIndex.git``

``cd findMinimumIndex/``


The following Will install the benchmark under /user/.local

``source buildGoogleBenchmark.sh``

edit

``cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DCMAKE_INSTALL_PREFIX=${HOME}/.local ../ ``

to change location


# Build with CMake 

``mkdir build; cd build``

``cmake ../FindMinimumIndex``

``make``

``./findMinimumIndex_bench --benchmark_report_aggregates_only=true --benchmark_repetitions=20``

# What is included so far

- "C" style (2 variations)
- "STL" style
- Using SSE intrinsics
- Using AVX2 intrinsics
- Using SSE/AVX2 via compiler vector extension
