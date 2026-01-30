#include "RandomFactory.h"
#include <chrono>

RandomFactory::RandomFactory()
    : RandomFactory(
        999L
      ) {}
    // : RandomFactory(
    //     std::chrono::duration_cast<std::chrono::milliseconds>(
    //         std::chrono::system_clock::now().time_since_epoch()
    //     ).count()
    //   ) {}

RandomFactory::RandomFactory(uint64_t seed)
    : rng(static_cast<uint32_t>(seed)),
      distDouble(0.0, 1.0),
      distFloat(0.0f, 1.0f) {}

double RandomFactory::nextDouble() {
    return distDouble(rng);
}

float RandomFactory::nextFloat() {
    return distFloat(rng);
}

std::vector<double> RandomFactory::batchDouble(size_t size) {
    std::vector<double> out(size);
    for (auto& v : out) {
        v = distDouble(rng);
    }
    return out;
}

std::vector<float> RandomFactory::batchFloat(size_t size) {
    std::vector<float> out(size);
    for (auto& v : out) {
        v = distFloat(rng);
    }
    return out;
}

void RandomFactory::fillDouble(double* array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        array[i] = distDouble(rng);
    }
}

void RandomFactory::fillFloat(float* array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        array[i] = distFloat(rng);
    }
}