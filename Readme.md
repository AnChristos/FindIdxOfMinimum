# FindMinimumIndex

Playground for using SIMD for calculating the Index of the minimum in a array of floats.

# If you do not have already google benchmark installed

``git clone https://github.com/google/benchmark.git``

The following script from this repo will install the benchmark under ``${HOME}/.local``

``source buildGoogleBenchmark.sh``

# Build FindMinimumIndex 

``git clone https://github.com/AnChristos/FindMinimumIndex.git``

``mkdir build; cd build``

``cmake ../FindMinimumIndex``

``make``

``./findMinimumIndex_bench --benchmark_report_aggregates_only=true --benchmark_repetitions=20``

