#ifndef FINDMINIMUMINDEX
#define FINDMINIMUMINDEX
#include "vec.h"
#include <algorithm>

inline int32_t
findMinIndexC(const float* distancesIn, int n)
{
  constexpr int alignment = 32;
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);
  float minvalue = array[0];
  int minIndex = 0;
  n &= -8;
  for (int i = 0; i < n; ++i) {
    const float value = array[i];
    if (value < minvalue) {
      minvalue = value;
      minIndex = i;
    }
  }
  return minIndex;
}

inline int32_t
findMinIndexC2(const float* distancesIn, int n)
{
  constexpr int alignment = 32;
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);
  int minIndex = 0;
  n &= -8;
  for (int i = 0; i < n; ++i) {
    minIndex = array[i] < array[minIndex] ? i : minIndex;
  }
  return minIndex;
}

inline int32_t
findMinIndexSTL(const float* distancesIn, int n)
{
  constexpr int alignment = 32;
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);
  n &= -8;
  return std::distance(array, std::min_element(array, array + n));
}

inline int32_t
findMinIndexVec(const float* distancesIn, int n)
{
  constexpr int alignment = 32;
  using namespace CxxUtils;
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);
  // Do 2 vectors of 4 elements , so 8 at time
  const vec<int, 4> increment = { 8, 8, 8, 8 };
  vec<int, 4> indices1 = { 0, 1, 2, 3 };
  vec<int, 4> indices2 = { 4, 5, 6, 7 };
  vec<int, 4> minindices1 = indices1;
  vec<int, 4> minindices2 = indices2;
  vec<float, 4> minvalues1;
  vec<float, 4> minvalues2;
  vload(minvalues1, array);
  vload(minvalues2, array + 4);
  vec<float, 4> values1;
  vec<float, 4> values2;
  for (int i = 8; i < n; i += 8) {
    // Load 8 elements at a time
    vload(values1, array + i);     // first 4
    vload(values2, array + i + 4); // second 4
    // 1
    indices1 = indices1 + increment;
    vec<int, 4> lt1 = values1 < minvalues1;
    vselect(minindices1, indices1, minindices1, lt1);
    vmin(minvalues1, values1, minvalues1);
    // 2
    indices2 = indices2 + increment;
    vec<int, 4> lt2 = values2 < minvalues2;
    vselect(minindices2, indices2, minindices2, lt2);
    vmin(minvalues2, values2, minvalues2);
  }
  // Compare //1 with //2
  vec<int, 4> lt = minvalues1 < minvalues2;
  vselect(minindices1, minindices1, minindices2, lt);
  vmin(minvalues1, minvalues1, minvalues2);
  /*
   * Do the final calculation scalar way
   */
  size_t minIndex = minindices1[0];
  float minvalue = minvalues1[0];
  for (size_t i = 1; i < 4; ++i) {
    const float value = minvalues1[i];
    if (value < minvalue) {
      minvalue = value;
      minIndex = minindices1[i];
    }
  }
  return minIndex;
}

#endif
