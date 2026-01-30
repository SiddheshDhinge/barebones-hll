#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

class UUIDFactory {
    static constexpr size_t BATCH_SIZE = 100'000;

    std::array<std::string, BATCH_SIZE> batch;
    size_t index{0};

    uint64_t high;
    uint64_t low;

    void generateBatch();
    void increment();
    static std::string toUUIDString(uint64_t high, uint64_t low);

public:
    UUIDFactory();

    std::string next();
    std::vector<std::string> getBatch(size_t size);
};