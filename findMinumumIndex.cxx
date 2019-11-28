#include <algorithm>
#include <random>
#include <chrono>
#include <iostream>

constexpr int alignment=32;

size_t findMinimumIndex1(float* __restrict arrayIn, const size_t n){  
  float* array = (float*)__builtin_assume_aligned(arrayIn, alignment);
  float minimum = array[0]; 
  size_t minIndex=0;
  for (size_t i=0 ; i<n ; ++i){ 
    if(array[i]<minimum){
      minimum=array[i];
      minIndex=i;
    }     
  }
  return minIndex;
} 


size_t findMinimumIndex2(float* __restrict arrayIn, const size_t n){  
  float* array = (float*)__builtin_assume_aligned(arrayIn, alignment);
  return std::distance(array, std::min_element(array, array+n));
}


int main(){

  //Fill array
  //std::random_device rd;
  //std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  std::mt19937 gen; //Standard mersenne_twister_engine seeded with rd()

  std::uniform_real_distribution<> dis(1.0, 10.0);
  std::uniform_int_distribution<> disint(70, 80);
  const size_t n= disint(gen);
  const size_t nn= n*(n-1)/2;
  alignas(alignment) float* array = new float[nn];
 
  for (size_t i = 0; i < nn; ++i) {
    // Use dis to transform the random unsigned int generated by gen into a
    // double. Each call to dis(gen) generates a new random double
    array[i]=dis(gen);
  }
 
   
  //1.
  { 
    //Test simple C-style solution
    std::cout << "--------------------------------" <<'\n';
    std::cout << "-- findMinimumIndex1 ---" <<'\n'; 
   //Time it
    std::chrono::steady_clock::time_point clock_begin = std::chrono::steady_clock::now();
    auto index=findMinimumIndex1(array,nn);
    std::chrono::steady_clock::time_point clock_end = std::chrono::steady_clock::now();
    std::chrono::steady_clock::duration diff = clock_end - clock_begin;
    //print 
    std::cout <<"Time: " << std::chrono::duration <double, std::nano> (diff).count() << "ns" << '\n';
    std::cout << "Minimum index " << index <<  " with value " <<array[index]<<'\n'; 
    std::cout << "--------------------------------" <<'\n';
  }
  
  //2.
  { 
    std::cout << "--------------------------------" <<'\n';
    std::cout << "--  findMinimumIndex2  ---" <<'\n'; 
   //Time it
    std::chrono::steady_clock::time_point clock_begin = std::chrono::steady_clock::now();
    auto index=findMinimumIndex2(array,nn);
    std::chrono::steady_clock::time_point clock_end = std::chrono::steady_clock::now();
    std::chrono::steady_clock::duration diff = clock_end - clock_begin;
    //print 
    std::cout <<"Time: " << std::chrono::duration <double, std::nano> (diff).count() << "ns" << '\n';
    std::cout << "Minimum index " << index <<  " with value " <<array[index]<<'\n'; 
    std::cout << "--------------------------------" <<'\n';
  }
  

  delete[] array;   
}
