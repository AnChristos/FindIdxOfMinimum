# FindIndexOfMinimum

Playground for using SIMD for calculating the Index of the minimum in a array of floats.

# If you do not have already google benchmark installed

``git clone https://github.com/google/benchmark.git``

The following script from this repo will install the benchmark under ``${HOME}/.local``

``source buildGoogleBenchmark.sh``

# Build FindIndexOfMinimum 

``git clone https://github.com/AnChristos/FindIndexOfMinimum.git``

``mkdir build; cd build``

``cmake ../FindIndexOfMinimum``

``make``

``./findIndexOfMinimum_bench --benchmark_report_aggregates_only=true --benchmark_repetitions=20``

# To compile for different arch modify the CMakeLists.txt

