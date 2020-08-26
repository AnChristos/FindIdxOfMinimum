# FindMinimumIndex-SIMD
Playground for using SIMD for the magnetic field cache interpolation

# First clone/install

Assuming you want also the google bench mark code

``git clone --recursive https://github.com/AnChristos/FindMinimumIndex.git``

``cd  cd findMinimumIndex-SIMD/``

``source buildGoogleBenchmark.sh``

# Run the test
Sanity test to see things working

``source build_test.sh ``

``./findMinimumIndex_test`` 

# Run the benchmark

``source buildBench.sh`` 

``./findMinimumIndex_bench``

#CMake

``mkdir build; cd build``
``cmake ../``
``cmake``
``./findMinimumIndex_bench``

# What is included so far

- "C" style (2 variations)
- "STL" style
- Using SSE intrinsics
- Using AVX2 intrinsics
- Using SSE/AVX2 via compiler vector extension
