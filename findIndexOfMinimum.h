#ifndef FINDMINIMUMINDEX
#define FINDMINIMUMINDEX
#include "vec.h"
#include <algorithm>

namespace findIndexOfMinimumDetail {

constexpr int alignment = 32;
// index of minimum scalar
[[gnu::always_inline]] inline int32_t
scalarC(const float* distancesIn, int n)
{
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);
  float minvalue = array[0];
  int minIndex = 0;
  for (int i = 0; i < n; ++i) {
    const float value = array[i];
    if (value < minvalue) {
      minvalue = value;
      minIndex = i;
    }
  }
  return minIndex;
}

// index of minimum STL
[[gnu::always_inline]] inline int32_t
scalarSTL(const float* distancesIn, int n)
{
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);
  return std::distance(array, std::min_element(array, array + n));
}

// index of minimum vectorized. Always tracking indices
[[gnu::always_inline]] inline int32_t
vecAlwaysTrackIdx(const float* distancesIn, int n)
{
  using namespace CxxUtils;
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);
  const vec<int, 4> increment = { 16, 16, 16, 16 };

  vec<int, 4> indices1 = { 0, 1, 2, 3 };
  vec<int, 4> indices2 = { 4, 5, 6, 7 };
  vec<int, 4> indices3 = { 8, 9, 10, 11 };
  vec<int, 4> indices4 = { 12, 13, 14, 15 };

  vec<int, 4> minIndices1 = indices1;
  vec<int, 4> minIndices2 = indices2;
  vec<int, 4> minIndices3 = indices3;
  vec<int, 4> minIndices4 = indices4;

  vec<float, 4> minValues1;
  vec<float, 4> minValues2;
  vec<float, 4> minValues3;
  vec<float, 4> minValues4;
  vload(minValues1, array);
  vload(minValues2, array + 4);
  vload(minValues3, array + 8);
  vload(minValues4, array + 12);

  vec<float, 4> values1;
  vec<float, 4> values2;
  vec<float, 4> values3;
  vec<float, 4> values4;
  for (int i = 16; i < n; i += 16) {
    // 1
    vload(values1, array + i); // 0-3
    indices1 = indices1 + increment;
    vec<int, 4> lt1 = values1 < minValues1;
    vselect(minIndices1, indices1, minIndices1, lt1);
    vmin(minValues1, values1, minValues1);
    // 2
    vload(values2, array + i + 4); // 4-7
    indices2 = indices2 + increment;
    vec<int, 4> lt2 = values2 < minValues2;
    vselect(minIndices2, indices2, minIndices2, lt2);
    vmin(minValues2, values2, minValues2);
    // 3
    vload(values3, array + i + 8); // 8-11
    indices3 = indices3 + increment;
    vec<int, 4> lt3 = values3 < minValues3;
    vselect(minIndices3, indices3, minIndices3, lt3);
    vmin(minValues3, values3, minValues3);
    // 4
    vload(values4, array + i + 12); // 12-15
    indices4 = indices4 + increment;
    vec<int, 4> lt4 = values4 < minValues4;
    vselect(minIndices4, indices4, minIndices4, lt4);
    vmin(minValues4, values4, minValues4);
  }

  float minValues[16];
  int minIndices[16];
  vstore(minValues, minValues1);
  vstore(minValues + 4, minValues2);
  vstore(minValues + 8, minValues3);
  vstore(minValues + 12, minValues4);
  vstore(minIndices, minIndices1);
  vstore(minIndices + 4, minIndices2);
  vstore(minIndices + 8, minIndices3);
  vstore(minIndices + 12, minIndices4);

  float minValue = minValues[0];
  int32_t minIndex = minIndices[0];
  for (size_t i = 1; i < 16; ++i) {
    const float value = minValues[i];
    const int32_t index = minIndices[i];
    if (value < minValue) {
      minValue = value;
      minIndex = index;
    } else if (value == minValue && index < minIndex) {
      // we want to return the smallest index
      // in case of 2 same values
      minIndex = index;
    }
  }
  return minIndex;
}

// index of minimum vectorized. Update indices in new minimum
[[gnu::always_inline]] inline int32_t
vecUpdateIdxOnNewMin(const float* distancesIn, int n)
{
  using namespace CxxUtils;
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);

  int32_t idx = 0;
  float min = distancesIn[0];
  vec<float, 4> minValues;
  vbroadcast(minValues, min);
  vec<float, 4> values1;
  vec<float, 4> values2;
  vec<float, 4> values3;
  vec<float, 4> values4;
  vec<float, 4> values5;
  vec<float, 4> values6;
  vec<float, 4> values7;
  vec<float, 4> values8;
  for (int i = 0; i < n; i += 32) {
    // 1
    vload(values1, array + i); // 0-3
    // 2
    vload(values2, array + i + 4); // 4-7
    // 3
    vload(values3, array + i + 8); // 8-11
    // 4
    vload(values4, array + i + 12); // 12-15
    // 5
    vload(values5, array + i + 16); // 16-19
    // 6
    vload(values6, array + i + 20); // 20-23
    // 7
    vload(values7, array + i + 24); // 24-27
    // 8
    vload(values8, array + i + 28); // 28-31

    // Compare //1 with //2
    vmin(values1, values1, values2);
    // compare //3 with //4
    vmin(values3, values3, values4);
    // Compare //5 with //6
    vmin(values5, values5, values6);
    // compare /7 with //8
    vmin(values7, values7, values8);

    // Compare //1 with //3
    vmin(values1, values1, values3);
    // Compare //5 with //7
    vmin(values5, values5, values7);
    // Compare //1 with //5
    vmin(values1, values1, values5);

    // see if the new minimum contain something less
    // than the existing.
    vec<int, 4> newMinimumMask = values1 < minValues;
    if (vany(newMinimumMask)) {
      idx = i;
      float minCandidates[4];
      vstore(minCandidates, values1);
      for (int j = 0; j < 4; ++j) {
        if (minCandidates[j] < min) {
          min = minCandidates[j];
        }
      }
      vbroadcast(minValues, min);
    }
  }
  /*
   * Do the final calculation scalar way
   */
  for (int i = idx; i < idx + 32; ++i) {
    if (distancesIn[i] == min) {
      return i;
    }
  }
  return 0;
}

[[gnu::always_inline]] inline float
vecFindMinimum(const float* distancesIn, int n)
{
  using namespace CxxUtils;
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);

  vec<float, 4> minValues1;
  vec<float, 4> minValues2;
  vec<float, 4> minValues3;
  vec<float, 4> minValues4;
  vec<float, 4> minValues5;
  vec<float, 4> minValues6;
  vec<float, 4> minValues7;
  vec<float, 4> minValues8;
  vload(minValues1, array);
  vload(minValues2, array + 4);
  vload(minValues3, array + 8);
  vload(minValues4, array + 12);
  vload(minValues5, array + 16);
  vload(minValues6, array + 20);
  vload(minValues7, array + 24);
  vload(minValues8, array + 28);
  vec<float, 4> values1;
  vec<float, 4> values2;
  vec<float, 4> values3;
  vec<float, 4> values4;
  vec<float, 4> values5;
  vec<float, 4> values6;
  vec<float, 4> values7;
  vec<float, 4> values8;
  for (int i = 32; i < n; i += 32) {
    // 1
    vload(values1, array + i); // 0-3
    vmin(minValues1, values1, minValues1);
    // 2
    vload(values2, array + i + 4); // 4-7
    vmin(minValues2, values2, minValues2);
    // 3
    vload(values3, array + i + 8); // 8-11
    vmin(minValues3, values3, minValues3);
    // 4
    vload(values4, array + i + 12); // 12-15
    vmin(minValues4, values4, minValues4);
    // 4
    vload(values5, array + i + 16); // 16-19
    vmin(minValues5, values5, minValues5);
    // 4
    vload(values6, array + i + 20); // 20-23
    vmin(minValues6, values6, minValues6);
    // 4
    vload(values7, array + i + 24); // 24-27
    vmin(minValues7, values7, minValues7);
    // 4
    vload(values8, array + i + 28); // 28-31
    vmin(minValues8, values8, minValues8);
  }
  // Compare //1 with //2
  vmin(minValues1, minValues1, minValues2);
  // compare //3 with //4
  vmin(minValues3, minValues3, minValues4);
  // compare //5 with //6
  vmin(minValues5, minValues5, minValues6);
  // compare //7 with //8
  vmin(minValues7, minValues7, minValues8);

  // Compare //1 with //3
  vmin(minValues1, minValues1, minValues3);
  // Compare //5 with //7
  vmin(minValues5, minValues5, minValues7);

  // Compare //1 with //5
  vmin(minValues1, minValues1, minValues5);

  // Do the final calculation scalar way
  float minValues[4];
  vstore(minValues, minValues1);
  float minvalue = minValues[0];
  for (size_t i = 1; i < 4; ++i) {
    const float value = minValues[i];
    if (value < minvalue) {
      minvalue = value;
    }
  }
  return minvalue;
}
[[gnu::always_inline]] inline int32_t
vecIdxofValue(const float value, const float* distancesIn, int n)
{
  using namespace CxxUtils;
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);

  vec<float, 4> values1;
  vec<float, 4> values2;
  vec<float, 4> values3;
  vec<float, 4> values4;
  vec<float, 4> target;
  vbroadcast(target, value);
  for (int i = 0; i < n; i += 16) {
    // 1
    vload(values1, array + i); // 0-3
    vec<int, 4> eq1 = values1 == target;
    // 2
    vload(values2, array + i + 4); // 4-7
    vec<int, 4> eq2 = values2 == target;
    // 3
    vload(values3, array + i + 8); // 8-11
    vec<int, 4> eq3 = values3 == target;
    // 4
    vload(values4, array + i + 12); // 12-15
    vec<int, 4> eq4 = values4 == target;

    vec<int, 4> eq12 = eq1 || eq2;
    vec<int, 4> eq34 = eq3 || eq4;
    vec<int, 4> eqAny = eq12 || eq34;
    if (vany(eqAny)) {
      for (int32_t idx = i; idx < i + 16; ++idx) {
        if (distancesIn[idx] == value) {
          return idx;
        }
      }
    }
  }
  return -1;
}

[[gnu::always_inline]] inline int32_t
vecMinThenIdx(const float* distancesIn, int n)
{
  using namespace CxxUtils;
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);
  const float min = vecFindMinimum(array, n);
  return vecIdxofValue(min, array, n);
}

} // findIndexOfMinimumDetail

namespace findIndexOfMinimum {
enum Impl
{
  VecUpdateIdxOnNewMin = 0,
  VecAlwaysTrackIdx = 1,
  VecMinThenIdx = 2,
  C = 3,
  STL = 4
};

template<enum Impl I>
[[gnu::always_inline]] inline int32_t
impl(const float* distancesIn, int n)
{

  static_assert(I == VecUpdateIdxOnNewMin || I == VecAlwaysTrackIdx ||
                  I == VecMinThenIdx || I == C || I == STL,
                "Not a valid implementation chosen");
  if constexpr (I == VecUpdateIdxOnNewMin) {
    return findIndexOfMinimumDetail::vecUpdateIdxOnNewMin(distancesIn, n);
  } else if constexpr (I == VecAlwaysTrackIdx) {
    return findIndexOfMinimumDetail::vecAlwaysTrackIdx(distancesIn, n);
  } else if constexpr (I == VecMinThenIdx) {
    return findIndexOfMinimumDetail::vecMinThenIdx(distancesIn, n);
  } else if constexpr (I == C) {
    return findIndexOfMinimumDetail::scalarC(distancesIn, n);
  } else if constexpr (I == STL) {
    return findIndexOfMinimumDetail::scalarSTL(distancesIn, n);
  }
}
} // findIndexOfMinimum
#endif
