g++ -O3   -march=native  -Wall -ftree-vectorize -std=c++17 KLGaussianMixtureReduction.cxx -c -o KLGaussianMixtureReductions.o
g++ testMergeComponents.cxx KLGaussianMixtureReductions.o -O3 -Wall  -std=c++17 -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread -o test
