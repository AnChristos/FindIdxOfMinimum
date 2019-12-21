Find Index of Mimimum  in array 
------------------------------------
Testing various implementation of finding the index of the minimum element in an array.
- "C" style
- "STL" style
- Using SSE2/SSE4.1 intrinsics 
- Using AVX2 intrinsics


clang8 with  -march=x86-64 -O2 example output
-------------------------------------
```
2019-12-21 12:35:18
Running ./mybenchmark
Run on (8 X 2500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 1.29, 1.48, 1.62
--------------------------------------------------------------------------------
Benchmark                                      Time             CPU   Iterations
--------------------------------------------------------------------------------
findMinimumIndexC/64                        61.3 ns         61.2 ns     11251487
findMinimumIndexC/512                        316 ns          316 ns      2182297
findMinimumIndexC/4096                      2371 ns         2369 ns       294877
findMinimumIndexC/8192                      4706 ns         4703 ns       152636
findMinimumIndexCNoVal/64                   37.2 ns         37.1 ns     18929970
findMinimumIndexCNoVal/512                   237 ns          236 ns      2827632
findMinimumIndexCNoVal/4096                 1783 ns         1782 ns       395132
findMinimumIndexCNoVal/8192                 3594 ns         3588 ns       194229
findMinimumIndexSTL/64                      31.6 ns         31.5 ns     22253731
findMinimumIndexSTL/512                      224 ns          223 ns      3096866
findMinimumIndexSTL/4096                    1681 ns         1679 ns       413611
findMinimumIndexSTL/8192                    3258 ns         3255 ns       214142
findMinimumIndexSSE_4/64                    19.8 ns         19.8 ns     35176586
findMinimumIndexSSE_4/512                    155 ns          155 ns      4520912
findMinimumIndexSSE_4/4096                  1248 ns         1246 ns       546111
findMinimumIndexSSE_4/8192                  2500 ns         2492 ns       282222
findMinimumIndexSSEBlendValues_4/64         30.6 ns         30.5 ns     23019652
findMinimumIndexSSEBlendValues_4/512         245 ns          245 ns      2789600
findMinimumIndexSSEBlendValues_4/4096       2042 ns         2039 ns       348531
findMinimumIndexSSEBlendValues_4/8192       4105 ns         4101 ns       169452
findMinimumIndexSSE_8/64                    20.2 ns         20.2 ns     34641118
findMinimumIndexSSE_8/512                    124 ns          124 ns      5612527
findMinimumIndexSSE_8/4096                   938 ns          937 ns       739309
findMinimumIndexSSE_8/8192                  1883 ns         1881 ns       369062
```




