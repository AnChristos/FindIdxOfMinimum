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


./mybenchmark --benchmark_report_aggregates_only=true --benchmark_repetitions=20

2020-02-12 18:33:57
Running ./mybenchmark
Run on (8 X 2500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 1.81, 1.78, 1.66
------------------------------------------------------------------------------
Benchmark                                    Time             CPU   Iterations
------------------------------------------------------------------------------
findMinimumIndexC/8_mean                  6.03 ns         6.00 ns           20
findMinimumIndexC/8_median                5.95 ns         5.94 ns           20
findMinimumIndexC/8_stddev               0.221 ns        0.185 ns           20
findMinimumIndexC/64_mean                 51.6 ns         51.5 ns           20
findMinimumIndexC/64_median               51.5 ns         51.5 ns           20
findMinimumIndexC/64_stddev              0.258 ns        0.245 ns           20
findMinimumIndexC/512_mean                 326 ns          325 ns           20
findMinimumIndexC/512_median               325 ns          325 ns           20
findMinimumIndexC/512_stddev              1.04 ns         1.02 ns           20
findMinimumIndexC/4096_mean               2423 ns         2420 ns           20
findMinimumIndexC/4096_median             2422 ns         2419 ns           20
findMinimumIndexC/4096_stddev             11.7 ns         11.2 ns           20
findMinimumIndexC/8192_mean               4775 ns         4769 ns           20
findMinimumIndexC/8192_median             4773 ns         4768 ns           20
findMinimumIndexC/8192_stddev             15.1 ns         14.7 ns           20
findMinimumIndexC2/8_mean                 4.32 ns         4.32 ns           20
findMinimumIndexC2/8_median               4.32 ns         4.32 ns           20
findMinimumIndexC2/8_stddev              0.018 ns        0.017 ns           20
findMinimumIndexC2/64_mean                24.8 ns         24.8 ns           20
findMinimumIndexC2/64_median              24.8 ns         24.7 ns           20
findMinimumIndexC2/64_stddev             0.134 ns        0.131 ns           20
findMinimumIndexC2/512_mean                193 ns          193 ns           20
findMinimumIndexC2/512_median              193 ns          193 ns           20
findMinimumIndexC2/512_stddev             1.37 ns         1.32 ns           20
findMinimumIndexC2/4096_mean              1414 ns         1412 ns           20
findMinimumIndexC2/4096_median            1413 ns         1411 ns           20
findMinimumIndexC2/4096_stddev            8.06 ns         7.19 ns           20
findMinimumIndexC2/8192_mean              2794 ns         2785 ns           20
findMinimumIndexC2/8192_median            2780 ns         2774 ns           20
findMinimumIndexC2/8192_stddev            39.6 ns         33.8 ns           20
findMinimumIndexSTL/8_mean                7.42 ns         7.39 ns           20
findMinimumIndexSTL/8_median              7.40 ns         7.37 ns           20
findMinimumIndexSTL/8_stddev             0.183 ns        0.159 ns           20
findMinimumIndexSTL/64_mean               30.9 ns         30.7 ns           20
findMinimumIndexSTL/64_median             30.5 ns         30.4 ns           20
findMinimumIndexSTL/64_stddev             1.09 ns        0.850 ns           20
findMinimumIndexSTL/512_mean               219 ns          218 ns           20
findMinimumIndexSTL/512_median             217 ns          216 ns           20
findMinimumIndexSTL/512_stddev            7.64 ns         6.39 ns           20
findMinimumIndexSTL/4096_mean             1661 ns         1654 ns           20
findMinimumIndexSTL/4096_median           1653 ns         1648 ns           20
findMinimumIndexSTL/4096_stddev           29.9 ns         25.5 ns           20
findMinimumIndexSTL/8192_mean             3243 ns         3235 ns           20
findMinimumIndexSTL/8192_median           3247 ns         3239 ns           20
findMinimumIndexSTL/8192_stddev           38.0 ns         35.2 ns           20
findMinimumIndexVector4/8_mean            5.24 ns         5.23 ns           20
findMinimumIndexVector4/8_median          5.23 ns         5.23 ns           20
findMinimumIndexVector4/8_stddev         0.016 ns        0.015 ns           20
findMinimumIndexVector4/64_mean           19.9 ns         19.9 ns           20
findMinimumIndexVector4/64_median         19.9 ns         19.9 ns           20
findMinimumIndexVector4/64_stddev        0.110 ns        0.108 ns           20
findMinimumIndexVector4/512_mean           159 ns          159 ns           20
findMinimumIndexVector4/512_median         159 ns          159 ns           20
findMinimumIndexVector4/512_stddev       0.445 ns        0.435 ns           20
findMinimumIndexVector4/4096_mean         1261 ns         1260 ns           20
findMinimumIndexVector4/4096_median       1259 ns         1258 ns           20
findMinimumIndexVector4/4096_stddev       9.06 ns         8.72 ns           20
findMinimumIndexVector4/8192_mean         2510 ns         2507 ns           20
findMinimumIndexVector4/8192_median       2508 ns         2505 ns           20
findMinimumIndexVector4/8192_stddev       9.48 ns         9.34 ns           20
findMinimumIndexAVX2/8_mean               6.45 ns         6.44 ns           20
findMinimumIndexAVX2/8_median             6.46 ns         6.45 ns           20
findMinimumIndexAVX2/8_stddev            0.046 ns        0.045 ns           20
findMinimumIndexAVX2/64_mean              18.6 ns         18.6 ns           20
findMinimumIndexAVX2/64_median            18.6 ns         18.6 ns           20
findMinimumIndexAVX2/64_stddev           0.035 ns        0.034 ns           20
findMinimumIndexAVX2/512_mean             92.8 ns         92.6 ns           20
findMinimumIndexAVX2/512_median           92.6 ns         92.5 ns           20
findMinimumIndexAVX2/512_stddev          0.299 ns        0.242 ns           20
findMinimumIndexAVX2/4096_mean             687 ns          685 ns           20
findMinimumIndexAVX2/4096_median           687 ns          686 ns           20
findMinimumIndexAVX2/4096_stddev          1.33 ns         1.11 ns           20
findMinimumIndexAVX2/8192_mean            1361 ns         1359 ns           20
findMinimumIndexAVX2/8192_median          1361 ns         1358 ns           20
findMinimumIndexAVX2/8192_stddev          1.96 ns         1.80 ns           20
findMinimumIndexSSE_4/8_mean              5.56 ns         5.55 ns           20
findMinimumIndexSSE_4/8_median            5.55 ns         5.55 ns           20
findMinimumIndexSSE_4/8_stddev           0.085 ns        0.080 ns           20
findMinimumIndexSSE_4/64_mean             20.8 ns         20.7 ns           20
findMinimumIndexSSE_4/64_median           20.8 ns         20.7 ns           20
findMinimumIndexSSE_4/64_stddev          0.081 ns        0.079 ns           20
findMinimumIndexSSE_4/512_mean             161 ns          161 ns           20
findMinimumIndexSSE_4/512_median           161 ns          161 ns           20
findMinimumIndexSSE_4/512_stddev         0.967 ns        0.934 ns           20
findMinimumIndexSSE_4/4096_mean           1277 ns         1275 ns           20
findMinimumIndexSSE_4/4096_median         1277 ns         1275 ns           20
findMinimumIndexSSE_4/4096_stddev         3.71 ns         3.50 ns           20
findMinimumIndexSSE_4/8192_mean           2549 ns         2544 ns           20
findMinimumIndexSSE_4/8192_median         2549 ns         2545 ns           20
findMinimumIndexSSE_4/8192_stddev         6.89 ns         6.80 ns           20
findMinimumIndexSSE_8/8_mean              4.16 ns         4.15 ns           20
findMinimumIndexSSE_8/8_median            4.15 ns         4.14 ns           20
findMinimumIndexSSE_8/8_stddev           0.030 ns        0.028 ns           20
findMinimumIndexSSE_8/64_mean             16.6 ns         16.6 ns           20
findMinimumIndexSSE_8/64_median           16.5 ns         16.5 ns           20
findMinimumIndexSSE_8/64_stddev          0.204 ns        0.189 ns           20
findMinimumIndexSSE_8/512_mean             116 ns          116 ns           20
findMinimumIndexSSE_8/512_median           116 ns          116 ns           20
findMinimumIndexSSE_8/512_stddev          1.13 ns         1.03 ns           20
findMinimumIndexSSE_8/4096_mean            902 ns          900 ns           20
findMinimumIndexSSE_8/4096_median          898 ns          897 ns           20
findMinimumIndexSSE_8/4096_stddev         14.0 ns         13.1 ns           20
findMinimumIndexSSE_8/8192_mean           1792 ns         1789 ns           20
findMinimumIndexSSE_8/8192_median         1789 ns         1786 ns           20
findMinimumIndexSSE_8/8192_stddev         13.1 ns         12.6 ns           20


```



