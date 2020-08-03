cd benchmark/
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release  -DBENCHMARK_ENABLE_GTEST_TESTS=OFF ../
make
cd ../../
