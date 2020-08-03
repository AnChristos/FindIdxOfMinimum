/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */

#ifndef TEST_VEC_H
#define TEST_VEC_H

#include <cstdlib>
#include <cstring>
#include <type_traits>
// Do we have the vector_size attribute for writing explicitly
// vectorized code?
#if (defined(__GNUC__) || defined(__clang__)) && !defined(__ICC) &&            \
   !defined(__COVERITY__) &&                                                   \
  !defined(__CUDACC__)
#define HAVE_VECTOR_SIZE_ATTRIBUTE 1
#else
#define HAVE_VECTOR_SIZE_ATTRIBUTE 0
#endif
namespace CxxUtils {
// Define a nice alias for a built-in vectorized type.
template<typename T, size_t N>
using vec __attribute__((vector_size(N * sizeof(T)))) = T;

/**
 * @brief Deduce the element type from a vectorized type.
 */
template<class VEC>
struct vec_type
{
  // Requires c++20.
  // typedef typename std::invoke_result< decltype([](const VEC& v){return
  // v[0];}), VEC& >::type type;

  // Works in c++17.
  static auto elt(const VEC& v) -> decltype(v[0]);
  typedef typename std::invoke_result<decltype(elt), const VEC&>::type type1;
  typedef std::remove_cv_t<std::remove_reference_t<type1>> type;
};

/// Deduce the element type from a vectorized type.
template<class VEC>
using vec_type_t = typename vec_type<VEC>::type;

/**
 * @brief Return the number of elements in a vectorized type.
 */
template<class VEC>
inline constexpr size_t
vec_size()
{
  typedef vec_type_t<VEC> ELT;
  return sizeof(VEC) / sizeof(ELT);
}

/**
 * @brief Return the number of elements in a vectorized type.
 */
template<class VEC>
inline constexpr size_t
vec_size(const VEC&)
{
  typedef vec_type_t<VEC> ELT;
  return sizeof(VEC) / sizeof(ELT);
}

/**
 * brief Deduce the type of a mask , type returned by relational operations,
 * for a vectorized type.
 */
template<class VEC>
struct mask_type
{
  static auto maskt(const VEC& v1, const VEC& v2) -> decltype(v1 < v2);
  typedef
    typename std::invoke_result<decltype(maskt), const VEC&, const VEC&>::type
      type1;
  typedef std::remove_cv_t<std::remove_reference_t<type1>> type;
};
template<class VEC>
/// Deduce the mask type for a vectorized type.
using mask_type_t = typename mask_type<VEC>::type;

/**
 * brief Copy a scalar to each element of a vectorized type.
 * Similar functionality to _mm_set/_mm_broadcast x86-64 intrinsics.
 */
template<typename VEC, typename T>
inline void
vbroadcast(VEC& v, T x)
{
  // This may look inefficient, but the loop goes away when we
  // compile with optimization.
  const size_t N = CxxUtils::vec_size<VEC>();
  for (size_t i = 0; i < N; i++) {
    v[i] = x;
  }
}

/*
 * @brief load elements from  memory address (C-array)
 * to a vectorized type. Similar to _mm_load intrinsics
 */
template<typename VEC>
inline void
vload(VEC& dst, vec_type_t<VEC>* mem_addr)
{
  std::memcpy(&dst, mem_addr, sizeof(VEC));
}

/*
 * @brief load elements from a vectorized type to
 * a memory address (C-array).
 * Similar to _mm_store intrinsics
 */
template<typename VEC>
inline void
vstore(vec_type_t<VEC>* mem_addr, VEC& src)
{
  std::memcpy(mem_addr, &src, sizeof(VEC));
}

/*
 * @brief select/blend function.
 * Similar _mm_blend X86-64 intrinsics
 */
template<typename VEC>
inline void
vselect(VEC& dst, const VEC& a, const VEC& b, const mask_type_t<VEC>& mask)
{
#if (defined(__clang__) && (__clang_major__ < 10)) ||                          \
  !HAVE_VECTOR_SIZE_ATTRIBUTE || WANT_VECTOR_FALLBACK
  constexpr size_t N = vec_size<VEC>();
  for (size_t i = 0; i < N; i++) {
    dst[i] = mask[i] ? a[i] : b[i];
  }
#else
  dst = mask ? a : b;
#endif
}

/*
 * @brief vectorized min.
 * Similar to _mm_min intrinsics
 */
template<typename VEC>
inline void
vmin(VEC& dst, const VEC& a, const VEC& b)
{
#if (defined(__clang__) && (__clang_major__ < 10)) ||                          \
  !HAVE_VECTOR_SIZE_ATTRIBUTE || WANT_VECTOR_FALLBACK
  constexpr size_t N = vec_size<VEC>();
  for (size_t i = 0; i < N; i++) {
    dst[i] = a[i] < b[i] ? a[i] : b[i];
  }
#else
  dst = a < b ? a : b;
#endif
}

} // namespace CxxUtils

#endif
