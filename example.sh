cd benchmark/
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release  -DBENCHMARK_ENABLE_GTEST_TESTS=OFF ../
make
cd ../../
g++ findMinumumIndex_bench.cxx -std=c++17 -march=native  -O3 -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -o mybenchmark
./mybenchmark --benchmark_report_aggregates_only=true --benchmark_repetitions=20
