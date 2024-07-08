#ifndef FINDMINIMUMINDEX
#define FINDMINIMUMINDEX
#include "vec.h"
#include <algorithm>
#include <memory>
#include <numeric>
#include <climits>

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

[[gnu::always_inline]] inline float
vecFindMinimum(const float* distancesIn, int n)
{
  using namespace CxxUtils;
  const float* array = std::assume_aligned<alignment>(distancesIn);

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
  }
  // Compare //1 with //2
  vmin(minValues1, minValues1, minValues2);
  // compare //3 with //4
  vmin(minValues3, minValues3, minValues4);
  // Compare //1 with //3
  vmin(minValues1, minValues1, minValues3);
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
vecIdxOfValue(const float value, const float* distancesIn, int n)
{
  using namespace CxxUtils;
  const float* array = std::assume_aligned<alignment>(distancesIn);

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
  const float* array = std::assume_aligned<alignment>(distancesIn);
  constexpr int blockSizePower2 = 8;
  constexpr int blockSize = 2 << blockSizePower2;
  // case for n less than blockSize
  if (n <= blockSize) {
    float min = vecFindMinimum(array, n);
    return vecIdxOfValue(min, array, n);
  }
  int32_t idx = 0;
  float min = array[0];
  // We might have a remainder that we need to handle
  const int remainder = n & (blockSize - 1);
  for (int32_t i = 0; i < (n - remainder); i += blockSize) {
    float mintmp = vecFindMinimum(array + i, blockSize);
    if (mintmp < min) {
      min = mintmp;
      idx = i;
    }
  }
  if (remainder != 0) {
    int index = n - remainder;
    float mintmp = vecFindMinimum(array + index, remainder);
    // if the minimu is here
    if (mintmp < min) {
      min = mintmp;
      return index + vecIdxOfValue(min, array + index, remainder);
    }
  }
  // otherwise no remainder
  return idx + vecIdxOfValue(min, array + idx, blockSize);
}

// TEMPLATED IMPLEMENTATION =========>
/* we can also attempt to template the last solution find index
 * and the then minimimum*/
#define IMPL_DO_PRAGMA( x ) _Pragma( #x )
#if defined( __clang__ )
#  define IMPL_LOOP_UNROLL( x ) IMPL_DO_PRAGMA( clang loop unroll_count( x ) )
#elif defined( __GNUC__ )
#  define IMPL_LOOP_UNROLL( x ) IMPL_DO_PRAGMA( GCC unroll x )
#else
#  define IMPL_LOOP_UNROLL( x )
#endif

// Functionalit similar to std::reduce for array of vector types.
// The reduction result is stored in array[0].
template <typename T, int N, int SIZE>
void vreduce(CxxUtils::vec<T, N> array[SIZE], const auto& lambda) {

  static_assert(SIZE != 0, "SIZE can not be 0");
  static_assert((SIZE & (SIZE - 1)) == 0 || SIZE == 1,
                "SIZE not 1 or a power of 2");
  if constexpr (SIZE == 1) {
    return;
  }
  for (int i = 0; i < SIZE / 2; i++) {
    lambda(array[i], array[i + (SIZE / 2)]);
  }
  if constexpr (SIZE / 2 > 0) {
    vreduce<T, N, SIZE / 2>(array, lambda);
  } else {
    return;
  }
}

/// ISA_WIDTH is the ISA width in bits e.g 128 for SSE
/// 256 for AVX2
///
/// STRIDE is how many elements (in units of elements) we want to cover
/// in each iteration
///
/// T is the element type
///
/// The input array is assumed to be at least
/// ISA_WIDTH/CHAR_BIT aligned e.g 16 for SSE4, 32 for AVX2
///
/// Based on the ISA and the element type
/// We choose the best size for the SIMD types
///
/// And then we use an array of as  many SIMD types needed
/// As to cover the stride.

template <int ISA_WIDTH, int STRIDE, typename T>
[[gnu::always_inline]] inline
T vFindMinimum(const T* distancesIn, int n) {
  using namespace CxxUtils;
  //We want to have vectors that fit on the specified
  //ISA
  //For large STRIDES we use an array of such
  //vectors
  constexpr int VEC_WIDTH = ISA_WIDTH / (sizeof(T) * CHAR_BIT);
  constexpr int ALIGNMENT = ISA_WIDTH / CHAR_BIT;
  constexpr int VECTOR_COUNT = STRIDE / VEC_WIDTH;
  static_assert((ISA_WIDTH & (ISA_WIDTH - 1)) == 0,
                "ISA_WIDTH not a power of 2");
  static_assert((STRIDE & (STRIDE - 1)) == 0,
                "STRIDE not a power of 2");
  static_assert((ALIGNMENT & (ALIGNMENT - 1)) == 0,
                "ALIGNMENT not a power of 2");
  static_assert(std::is_floating_point_v<T> || std::is_integral_v<T>, "T not a floating or integral type");

  const T* array = std::assume_aligned<ALIGNMENT>(distancesIn);

  using vec_T = vec<T, VEC_WIDTH>;
  vec_T minValues[VECTOR_COUNT];
  // Limit unrolling to 4 for now as too much unrolling can
  // also cause problems.
  // When VECTOR_COUNT is less than 4 the relevant loops
  // are removed
  IMPL_LOOP_UNROLL(4)
  for (int i = 0; i < VECTOR_COUNT; i++) {
    vload(minValues[i], array + (VEC_WIDTH * i));
  }
  vec_T values[VECTOR_COUNT];
  for (int i = STRIDE; i < n; i += STRIDE) {
    IMPL_LOOP_UNROLL(4)
    for (int j = 0; j < VECTOR_COUNT; ++j) {
      vload(values[j], array + i + (VEC_WIDTH * j));
      vmin(minValues[j], values[j], minValues[j]);
    }
  }

  vreduce<T, VEC_WIDTH, VECTOR_COUNT>(
      minValues,
      [](vec<T, VEC_WIDTH>& a, vec<T, VEC_WIDTH>& b) { a = a < b ? a : b; });

  T finalMinValues[VEC_WIDTH];
  vstore(finalMinValues, minValues[0]);

  // Do the final calculation scalar way
  return std::reduce(std::begin(finalMinValues),
                     std::end(finalMinValues),
                     finalMinValues[0],
                     [](float a, float b){ return a < b ? a : b; });
}

template <int ISA_WIDTH, int STRIDE, typename T>
 [[gnu::always_inline]] inline
int vIdxOfValue(const T value,
                const T* distancesIn, int n) {
  using namespace CxxUtils;
  //We want to have vectors that fit on the specified
  //ISA
  //For large STRIDES we use an array of such
  //vectors
  constexpr int VEC_WIDTH = ISA_WIDTH / (sizeof(T) * CHAR_BIT);
  constexpr int ALIGNMENT = ISA_WIDTH / CHAR_BIT;
  constexpr int VECTOR_COUNT = STRIDE / VEC_WIDTH;
  static_assert((ISA_WIDTH & (ISA_WIDTH - 1)) == 0,
                "ISA_WIDTH not a power of 2");
  static_assert((STRIDE & (STRIDE - 1)) == 0,
                "STRIDE not a power of 2");
  static_assert((ALIGNMENT & (ALIGNMENT - 1)) == 0,
                "ALIGNMENT not a power of 2");
  static_assert(std::is_floating_point_v<T> || std::is_integral_v<T>, "T not a floating or integral type");

  const T* array = std::assume_aligned<ALIGNMENT>(distancesIn);

  using vec_T = vec<T, VEC_WIDTH>;
  vec_T values[VECTOR_COUNT];
  vec_T target;
  vbroadcast(target, value);
  using vec_mask = vec_mask_type_t<vec<T, VEC_WIDTH>>;
  vec_mask eqs[VECTOR_COUNT];

  for (int i = 0; i < n; i += STRIDE) {
    IMPL_LOOP_UNROLL(4)
    for (int j = 0; j < VECTOR_COUNT; j++) {
      vload(values[j], array + i + (VEC_WIDTH * j));
      eqs[j] = values[j] == target;
    }

    vreduce<vec_type_t<vec_mask>, VEC_WIDTH, VECTOR_COUNT>(
        eqs, [](vec_mask& a, vec_mask& b) { a = a || b; });

    // See if we have the value in any
    // of the vectors
    // If yes then use scalar code to locate it
    if (vany(eqs[0])) {
      for (int idx = i; idx < i + STRIDE; ++idx) {
        if (distancesIn[idx] == value) {
          return idx;
        }
      }
    }
  }
  return -1;
}

template <int ISA_WIDTH, int STRIDE, typename T>
 [[gnu::always_inline]] inline
int vIdxOfMin(const T* distancesIn, int n) {
  using namespace CxxUtils;
  constexpr int ALIGNMENT = ISA_WIDTH / CHAR_BIT;
  static_assert((ISA_WIDTH & (ISA_WIDTH - 1)) == 0,
                "ISA_WIDTH not a power of 2");
  static_assert((STRIDE & (STRIDE - 1)) == 0,
                "STRIDE not a power of 2");
  static_assert((ALIGNMENT & (ALIGNMENT - 1)) == 0,
                "ALIGNMENT not a power of 2");
  static_assert(std::is_floating_point_v<T> || std::is_integral_v<T>, "T not a floating or integral type");

  const T* array = std::assume_aligned<ALIGNMENT>(distancesIn);
  // Finding of minimum needs to loop over all elements
  // But we can run the finding of index only inside a block
  constexpr int blockSize = 512;
  // case for n less than blockSize
  if (n <= blockSize) {
    T min = vFindMinimum<ISA_WIDTH, STRIDE>(array, n);
    return vIdxOfValue<ISA_WIDTH, STRIDE>(min, array, n);
  }
  int idx = 0;
  T min = array[0];
  // We might have a remainder that we need to handle
  const int remainder = n & (blockSize - 1);
  for (int i = 0; i < (n - remainder); i += blockSize) {
    T mintmp = vFindMinimum<ISA_WIDTH, STRIDE>(array + i, blockSize);
    if (mintmp < min) {
      min = mintmp;
      idx = i;
    }
  }
  if (remainder != 0) {
    int index = n - remainder;
    T mintmp = vFindMinimum<ISA_WIDTH, STRIDE>(array + index, remainder);
    // if the minimum is in this part
    if (mintmp < min) {
      min = mintmp;
      return index + vIdxOfValue<ISA_WIDTH, STRIDE>(min, array + index, remainder);
    }
  }
  //default return
  return idx + vIdxOfValue<ISA_WIDTH, STRIDE>(min, array + idx, blockSize);
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
    return findIndexOfMinimumDetail::vecMinThenIdx(distancesIn, n);
  } else if constexpr (I == VecMinThenIdxT) {
#if defined(__AVX2__)
    return findIndexOfMinimumDetail::vIdxOfMin<256, 16>(distancesIn, n);
#else
    return findIndexOfMinimumDetail::vIdxOfMin<128, 16>(distancesIn, n);
#endif
  } else if constexpr (I == C) {
    return findIndexOfMinimumDetail::scalarC(distancesIn, n);
  } else if constexpr (I == STL) {
    return findIndexOfMinimumDetail::scalarSTL(distancesIn, n);
  }
}
} // findIndexOfMinimum
#endif
