#pragma once

#include <random>
#include <vector>

class RandomFactory {
    std::mt19937 rng;
    std::uniform_real_distribution<double> distDouble;
    std::uniform_real_distribution<float> distFloat;

public:
    RandomFactory();
    explicit RandomFactory(uint64_t seed);

    double nextDouble();
    float nextFloat();

    std::vector<double> batchDouble(size_t size);
    std::vector<float> batchFloat(size_t size);

    void fillDouble(double* array, size_t size);
    void fillFloat(float* array, size_t size);
};