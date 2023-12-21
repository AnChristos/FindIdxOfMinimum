#include "findIndexOfMinimum.h"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <random>
constexpr int alignment = 32;
constexpr size_t nstart = 32;
constexpr size_t nend = 4096;
struct InitArray
{
public:
  InitArray()
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.1, 10.0);
    // create buffer of right size,properly aligned
    size_t const size = nend * sizeof(float);
    posix_memalign((void**)&array, alignment, size);
    for (size_t i = 0; i < nend; ++i) {
      array[i] = dis(gen);
    }
    // in case we want to sort
    //std::sort(array, array + nend, [](float a, float b) { return a > b; });
  }
  ~InitArray() { free(array); }
  float* array;
};
static const InitArray initArray;

static void
findIdxMinC(benchmark::State& state)
{
  for (auto _ : state) {
    const int n = state.range(0);
    int32_t minIndex =
      findIndexOfMinimum::impl<findIndexOfMinimum::C>(initArray.array, n);
    benchmark::DoNotOptimize(&minIndex);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(findIdxMinC)->RangeMultiplier(2)->Range(nstart, nend);

static void
findIdxMinSTL(benchmark::State& state)
{
  for (auto _ : state) {
    const int n = state.range(0);
    int32_t minIndex =
      findIndexOfMinimum::impl<findIndexOfMinimum::STL>(initArray.array, n);
    benchmark::DoNotOptimize(&minIndex);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(findIdxMinSTL)->RangeMultiplier(2)->Range(nstart, nend);

static void
findIdxMinVecAlwaysTrackIdx(benchmark::State& state)
{
  for (auto _ : state) {
    const int n = state.range(0);
    int32_t minIndex =
      findIndexOfMinimum::impl<findIndexOfMinimum::VecAlwaysTrackIdx>(
        initArray.array, n);
    benchmark::DoNotOptimize(&minIndex);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(findIdxMinVecAlwaysTrackIdx)->RangeMultiplier(2)->Range(nstart, nend);

static void
findIdxMinVecUpdateIdxOnNewMin(benchmark::State& state)
{
  for (auto _ : state) {
    const int n = state.range(0);
    int32_t minIndex =
      findIndexOfMinimum::impl<findIndexOfMinimum::VecUpdateIdxOnNewMin>(
        initArray.array, n);
    benchmark::DoNotOptimize(&minIndex);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(findIdxMinVecUpdateIdxOnNewMin)
  ->RangeMultiplier(2)
  ->Range(nstart, nend);

static void
findVecMinThenIdx(benchmark::State& state)
{
  for (auto _ : state) {
    const int n = state.range(0);
    int32_t minIndex =
      findIndexOfMinimum::impl<findIndexOfMinimum::VecMinThenIdx>(
        initArray.array, n);
    benchmark::DoNotOptimize(&minIndex);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(findVecMinThenIdx)->RangeMultiplier(2)->Range(nstart, nend);

BENCHMARK_MAIN();
