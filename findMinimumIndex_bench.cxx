﻿#include "findMinimumIndex.h"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <random>
/*
 * create global data
 * a bit hacky way
 */
constexpr size_t nstart = 32;
constexpr size_t nn = 16 << 9;

constexpr int alignment = 32;
constexpr size_t n = 16 << 8;
struct InitArray
{
public:
  InitArray()
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.1, 100.0);
    // create buffer of right size,properly aligned
    size_t const size = n * sizeof(float);
    posix_memalign((void**)&array, alignment, size);
    for (size_t i = 0; i < n; ++i) {
      // Use dis to transform the random unsigned int generated by gen into a
      // double. Each call to dis(gen) generates a new random double
      array[i] = dis(gen);
    }
  }
  ~InitArray() { free(array); }
  float* array;
};
static const InitArray initArray;

static void
findMinimumIndexC(benchmark::State& state)
{
  for (auto _ : state) {
    const int n = state.range(0);
    int32_t minIndex =
      findMinimumIndex::impl<findMinimumIndex::C>(initArray.array, n);
    benchmark::DoNotOptimize(&minIndex);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(findMinimumIndexC)->RangeMultiplier(8)->Range(nstart, nn);

static void
findMinimumIndexSTL(benchmark::State& state)
{
  for (auto _ : state) {
    const int n = state.range(0);
    int32_t minIndex =
      findMinimumIndex::impl<findMinimumIndex::STL>(initArray.array, n);
    benchmark::DoNotOptimize(&minIndex);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(findMinimumIndexSTL)->RangeMultiplier(8)->Range(nstart, nn);

static void
findMinimumIndexVecBlend(benchmark::State& state)
{
  for (auto _ : state) {
    const int n = state.range(0);
    int32_t minIndex =
      findMinimumIndex::impl<findMinimumIndex::VecBlend>(initArray.array, n);
    benchmark::DoNotOptimize(&minIndex);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(findMinimumIndexVecBlend)->RangeMultiplier(8)->Range(nstart, nn);

static void
findMinimumIndexVecUnordered(benchmark::State& state)
{
  for (auto _ : state) {
    const int n = state.range(0);
    int32_t minIndex = findMinimumIndex::impl<findMinimumIndex::VecUnordered>(
      initArray.array, n);
    benchmark::DoNotOptimize(&minIndex);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(findMinimumIndexVecUnordered)->RangeMultiplier(8)->Range(nstart, nn);

BENCHMARK_MAIN();
