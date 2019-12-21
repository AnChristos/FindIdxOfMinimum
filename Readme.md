Find Index of Mimimum  in array 
------------------------------------
Testing various implementation of finding the index of the minimum element in an array.
- "C" style
- "STL" style
- Using SSE2/SSE4.1 intrinsics 
- Using AVX2 intrinsics


clang8 example ouput
-------------------------------------
```
clang++ findMinumumIndex_bench.cxx -std=c++17 -march=native  -O3 -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -o mybenchmark
findMinumumIndex_bench.cxx:94:2: warning: ( "AVX2" ) [-W#warnings]
#warning( "AVX2" )
 ^
findMinumumIndex_bench.cxx:156:2: warning: ( "SSE4_1" ) [-W#warnings]
#warning( "SSE4_1" )
..........
..........
2019-12-21 15:08:02
Running ./mybenchmark
Run on (8 X 2500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 1.59, 1.80, 1.90
--------------------------------------------------------------------------------
Benchmark                                      Time             CPU   Iterations
--------------------------------------------------------------------------------
findMinimumIndexC/64                        50.5 ns         50.4 ns     13554334
findMinimumIndexC/512                        316 ns          316 ns      2173198
findMinimumIndexC/4096                      2406 ns         2402 ns       293234
findMinimumIndexC/8192                      4705 ns         4701 ns       148357
findMinimumIndexCNoVal/64                   24.5 ns         24.5 ns     28932674
findMinimumIndexCNoVal/512                   191 ns          191 ns      3673326
findMinimumIndexCNoVal/4096                 1368 ns         1367 ns       505642
findMinimumIndexCNoVal/8192                 2678 ns         2677 ns       256772
findMinimumIndexSTL/64                      29.4 ns         29.4 ns     23620878
findMinimumIndexSTL/512                      209 ns          208 ns      3335986
findMinimumIndexSTL/4096                    1611 ns         1610 ns       435489
findMinimumIndexSTL/8192                    3156 ns         3154 ns       219377
findMinimumIndexAVX2/64                     18.5 ns         18.5 ns     38010632
findMinimumIndexAVX2/512                    91.9 ns         91.8 ns      7618053
findMinimumIndexAVX2/4096                    683 ns          681 ns      1025085
findMinimumIndexAVX2/8192                   1350 ns         1348 ns       518522
findMinimumIndexSSE_4/64                    20.2 ns         20.2 ns     34335269
findMinimumIndexSSE_4/512                    157 ns          157 ns      4441343
findMinimumIndexSSE_4/4096                  1246 ns         1246 ns       553727
findMinimumIndexSSE_4/8192                  2478 ns         2476 ns       280094
findMinimumIndexSSEBlendValues_4/64         20.5 ns         20.4 ns     34381477
findMinimumIndexSSEBlendValues_4/512         158 ns          157 ns      4369320
findMinimumIndexSSEBlendValues_4/4096       1252 ns         1250 ns       562570
findMinimumIndexSSEBlendValues_4/8192       2479 ns         2477 ns       277343
findMinimumIndexSSE_8/64                    19.4 ns         19.4 ns     36333814
findMinimumIndexSSE_8/512                    116 ns          116 ns      5992176
findMinimumIndexSSE_8/4096                   880 ns          879 ns       778617
findMinimumIndexSSE_8/8192                  1750 ns         1748 ns       395364
```



