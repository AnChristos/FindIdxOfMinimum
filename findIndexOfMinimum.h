#ifndef FINDMINIMUMINDEX
#define FINDMINIMUMINDEX
#include "vec.h"
#include <algorithm>
#include <climits>
#include <memory>
#include <numeric>

namespace findIndexOfMinimumDetail {

constexpr int alignment = 32;
// index of minimum scalar
[[gnu::always_inline]] inline int32_t
scalarC(const float* distancesIn, int n)
{
  const float* array = std::assume_aligned<alignment>(distancesIn);
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
  const float* array = std::assume_aligned<alignment>(distancesIn);
  return std::distance(array, std::min_element(array, array + n));
}

// index of minimum vectorized. Always tracking indices
[[gnu::always_inline]] inline int32_t
vecAlwaysTrackIdx(const float* distancesIn, int n)
{
  using namespace CxxUtils;
  const float* array = std::assume_aligned<alignment>(distancesIn);
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
  const float* array = std::assume_aligned<alignment>(distancesIn);

  int32_t idx = 0;
  float min = distancesIn[0];
  vec<float, 4> minValues;
  vbroadcast(minValues, min);
  vec<float, 4> values1;
  vec<float, 4> values2;
  vec<float, 4> values3;
  vec<float, 4> values4;
  for (int i = 0; i < n; i += 16) {
    // 1
    vload(values1, array + i); // 0-3
    // 2
    vload(values2, array + i + 4); // 4-7
    // 3
    vload(values3, array + i + 8); // 8-11
    // 4
    vload(values4, array + i + 12); // 12-15

    // Compare //1 with //2
    vmin(values1, values1, values2);
    // compare //3 with //4
    vmin(values3, values3, values4);
    // Compare //1 with //3
    vmin(values1, values1, values3);
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
  for (int i = idx; i < idx + 16; ++i) {
    if (distancesIn[i] == min) {
      return i;
    }
  }
  return 0;
}

template<int ISA_WIDTH, typename T>
[[gnu::always_inline]] inline float
vecFindMinimum(const T* distancesIn, int n)
{
  using namespace CxxUtils;
  constexpr int VEC_WIDTH = ISA_WIDTH / (sizeof(T) * CHAR_BIT);
  constexpr int ALIGNMENT = ISA_WIDTH / CHAR_BIT;
  constexpr int STRIDE = 4 * VEC_WIDTH;
  static_assert((ISA_WIDTH & (ISA_WIDTH - 1)) == 0,
                "ISA_WIDTH not a power of 2");
  static_assert((STRIDE & (STRIDE - 1)) == 0, "STRIDE not a power of 2");
  static_assert((ALIGNMENT & (ALIGNMENT - 1)) == 0,
                "ALIGNMENT not a power of 2");
  static_assert(std::is_floating_point_v<T>, "T not a floating point type");

  const T* array = std::assume_aligned<ALIGNMENT>(distancesIn);
  using vec_t = vec<T, VEC_WIDTH>;
  vec_t minValues1;
  vec_t minValues2;
  vec_t minValues3;
  vec_t minValues4;
  vload(minValues1, array);
  vload(minValues2, array + VEC_WIDTH);
  vload(minValues3, array + VEC_WIDTH * 2);
  vload(minValues4, array + VEC_WIDTH * 3);
  vec_t values1;
  vec_t values2;
  vec_t values3;
  vec_t values4;
  for (int i = STRIDE; i < n; i += STRIDE) {
    // 1
    vload(values1, array + i);
    vmin(minValues1, values1, minValues1);
    // 2
    vload(values2, array + i + VEC_WIDTH);
    vmin(minValues2, values2, minValues2);
    // 3
    vload(values3, array + i + 2 * VEC_WIDTH);
    vmin(minValues3, values3, minValues3);
    // 4
    vload(values4, array + i + 3 * VEC_WIDTH);
    vmin(minValues4, values4, minValues4);
  }
  // Compare //1 with //2
  vmin(minValues1, minValues1, minValues2);
  // compare //3 with //4
  vmin(minValues3, minValues3, minValues4);
  // Compare //1 with //3
  vmin(minValues1, minValues1, minValues3);
  // Do the final calculation scalar way
  T finalMinValues[VEC_WIDTH];
  vstore(finalMinValues, minValues1);

  // Do the final calculation scalar way
  return std::reduce(std::begin(finalMinValues),
                     std::end(finalMinValues),
                     finalMinValues[0],
                     [](float a, float b) { return a < b ? a : b; });

}

template<int ISA_WIDTH, typename T>
[[gnu::always_inline]] inline int32_t
vecIdxOfValue(const T value, const T* distancesIn, int n)
{
  using namespace CxxUtils;
  constexpr int VEC_WIDTH = ISA_WIDTH / (sizeof(T) * CHAR_BIT);
  constexpr int ALIGNMENT = ISA_WIDTH / CHAR_BIT;
  constexpr int STRIDE = 4 * VEC_WIDTH;
  static_assert((ISA_WIDTH & (ISA_WIDTH - 1)) == 0,
                "ISA_WIDTH not a power of 2");
  static_assert((STRIDE & (STRIDE - 1)) == 0, "STRIDE not a power of 2");
  static_assert((ALIGNMENT & (ALIGNMENT - 1)) == 0,
                "ALIGNMENT not a power of 2");
  static_assert(std::is_floating_point_v<T>, "T not a floating point type");

  const T* array = std::assume_aligned<ALIGNMENT>(distancesIn);
  using vec_t = vec<T, VEC_WIDTH>;
  using vec_mask = vec_mask_type_t<vec_t>;
  vec_t values1;
  vec_t values2;
  vec_t values3;
  vec_t values4;
  vec_t target;
  vbroadcast(target, value);
  for (int i = 0; i < n; i += STRIDE) {
    // 1
    vload(values1, array + i);
    vec_mask eq1 = values1 == target;
    // 2
    vload(values2, array + i + VEC_WIDTH);
    vec_mask eq2 = values2 == target;
    // 3
    vload(values3, array + i + VEC_WIDTH * 2);
    vec_mask eq3 = values3 == target;
    // 4
    vload(values4, array + i + VEC_WIDTH * 3);
    vec_mask eq4 = values4 == target;

    vec_mask eq12 = eq1 || eq2;
    vec_mask eq34 = eq3 || eq4;
    vec_mask eqAny = eq12 || eq34;
    if (vany(eqAny)) {
      for (int32_t idx = i; idx < i + STRIDE; ++idx) {
        if (distancesIn[idx] == value) {
          return idx;
        }
      }
    }
  }
  return -1;
}

template<int ISA_WIDTH, typename T>
[[gnu::always_inline]] inline int32_t
vecMinThenIdx(const T* distancesIn, int n)
{
  using namespace CxxUtils;
  constexpr int VEC_WIDTH = ISA_WIDTH / (sizeof(T) * CHAR_BIT);
  constexpr int ALIGNMENT = ISA_WIDTH / CHAR_BIT;
  constexpr int STRIDE = 4 * VEC_WIDTH;
 static_assert((ISA_WIDTH & (ISA_WIDTH - 1)) == 0,
                "ISA_WIDTH not a power of 2");
  static_assert((STRIDE & (STRIDE - 1)) == 0, "STRIDE not a power of 2");
  static_assert((ALIGNMENT & (ALIGNMENT - 1)) == 0,
                "ALIGNMENT not a power of 2");
  static_assert(std::is_floating_point_v<T>, "T not a floating point type");

  const T* array = std::assume_aligned<ALIGNMENT>(distancesIn);
  constexpr int blockSize = 512;
  // case for n less than blockSize
  if (n <= blockSize) {
    T min = vecFindMinimum<ISA_WIDTH>(array, n);
    return vecIdxOfValue<ISA_WIDTH>(min, array, n);
  }
  int32_t idx = 0;
  T min = array[0];
  // We might have a remainder that we need to handle
  const int remainder = n & (blockSize - 1);
  for (int32_t i = 0; i < (n - remainder); i += blockSize) {
    T mintmp = vecFindMinimum<ISA_WIDTH>(array + i, blockSize);
    if (mintmp < min) {
      min = mintmp;
      idx = i;
    }
  }
  if (remainder != 0) {
    int index = n - remainder;
    float mintmp = vecFindMinimum<ISA_WIDTH>(array + index, remainder);
    // if the minimu is here
    if (mintmp < min) {
      min = mintmp;
      return index + vecIdxOfValue<ISA_WIDTH>(min, array + index, remainder);
    }
  }
  // otherwise no remainder
  return idx + vecIdxOfValue<ISA_WIDTH>(min, array + idx, blockSize);
}

} // findIndexOfMinimumDetail

namespace findIndexOfMinimum {
enum Impl
{
  VecUpdateIdxOnNewMin = 0,
  VecAlwaysTrackIdx = 1,
  VecMinThenIdx = 2,
  VecMinThenIdxT = 3,
  C = 4,
  STL = 5
};

template<enum Impl I>
[[gnu::always_inline]] inline int32_t
impl(const float* distancesIn, int n)
{
  if constexpr (I == VecUpdateIdxOnNewMin) {
    return findIndexOfMinimumDetail::vecUpdateIdxOnNewMin(distancesIn, n);
  } else if constexpr (I == VecAlwaysTrackIdx) {
    return findIndexOfMinimumDetail::vecAlwaysTrackIdx(distancesIn, n);
  } else if constexpr (I == VecMinThenIdx) {
#if defined(__AVX2__)
    return findIndexOfMinimumDetail::vecMinThenIdx<256>(distancesIn, n);
#else
    return findIndexOfMinimumDetail::vecMinThenIdx<128>(distancesIn, n);
#endif
  } else if constexpr (I == C) {
    return findIndexOfMinimumDetail::scalarC(distancesIn, n);
  } else if constexpr (I == STL) {
    return findIndexOfMinimumDetail::scalarSTL(distancesIn, n);
  }
}
} // findIndexOfMinimum
#endif
