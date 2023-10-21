﻿#include "findMinimumIndex.h"
#include <algorithm>
#include <iostream>
#include <random>

/*
 * Alignment of  64 bytes
 */
constexpr int alignment = 64;
/*
 * create global data
 * a bit hacky way
 */
constexpr size_t n = 16 << 8;
float* array;
class InitArray
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
};
InitArray initArray;

/*
 * Scalar code C style
 */
static void
findMinimumC()
{
  float min = findMinC(array, n);
  std::cout << "C Minimum value : " << min << '\n';
}

static void
findMinimumIndexC()
{
  int32_t minIndex = findMinIndexC(array, n);
  std::cout << "C Minimum index : " << minIndex << " with value "
            << array[minIndex] << '\n';
}

/*
 * Scalar code using STL
 */
static void
findMinimumSTL()
{
  float min = findMinSTL(array, n);
  std::cout << "STL Minimum value : " << min << '\n';
}

static void
findMinimumIndexSTL()
{
  int32_t minIndex = findMinIndexSTL(array, n);
  std::cout << "STL Minimum index : " << minIndex << " with value "
            << array[minIndex] << '\n';
}
/*
 * vec just find the minimum
 */
static void
findMinimumVec()
{
  float min = findMinVec(array, n);
  std::cout << "Vec Minimum value : " << min << '\n';
}

/*
 * vec find the minimum and index
 */
static void
findMinimumIndexVec()
{
  int32_t minIndex = findMinIndexVec(array, n);
  std::cout << "Vec Minimum index : " << minIndex << " with value "
            << array[minIndex] << '\n';
}

int
main()
{
  findMinimumC();
  findMinimumIndexC();
  findMinimumSTL();
  findMinimumIndexSTL();
  findMinimumVec();
  findMinimumIndexVec();
  return 0;
}
