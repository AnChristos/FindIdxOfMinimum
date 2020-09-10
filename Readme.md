# FindMinimumIndex-SIMD
Playground for using SIMD for calculating the Index of the minimum in a array of floats.

# Clone

Assuming you want also the google bench mark code

``git clone --recursive https://github.com/AnChristos/FindMinimumIndex.git``

``cd  cd findMinimumIndex-SIMD/``

``source buildGoogleBenchmark.sh``

else

``git clone https://github.com/AnChristos/FindMinimumIndex.git``

# Installation via CMake 

``mkdir build; cd build``

``cmake ../``

``make``

``./findMinimumIndex_bench``

# What is included so far

- "C" style (2 variations)
- "STL" style
- Using SSE intrinsics
- Using AVX2 intrinsics
- Using SSE/AVX2 via compiler vector extension
