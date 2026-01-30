#include "UUIDFactory.h"
#include <random>
#include <sstream>
#include <iomanip>

UUIDFactory::UUIDFactory() {
    // Generate initial random 128-bit value (Java UUID.randomUUID())
    std::random_device rd;
    std::mt19937_64 gen(rd());
    high = 100; //gen();
    low  = 100; //gen();

    generateBatch();
}

void UUIDFactory::increment() {
    low++;
    if (low == 0) { // overflow
        high++;
    }
}

std::string UUIDFactory::toUUIDString(uint64_t high, uint64_t low) {
    // Format: 8-4-4-4-12 hex (RFC 4122)
    std::ostringstream oss;
    oss << std::hex << std::setfill('0')
        << std::setw(8)  << (uint32_t)(high >> 32) << "-"
        << std::setw(4)  << (uint16_t)(high >> 16) << "-"
        << std::setw(4)  << (uint16_t)(high)       << "-"
        << std::setw(4)  << (uint16_t)(low >> 48)  << "-"
        << std::setw(12) << (uint64_t)(low & 0x0000FFFFFFFFFFFFULL);
    return oss.str();
}

void UUIDFactory::generateBatch() {
    for (size_t i = 0; i < BATCH_SIZE; i++) {
        batch[i] = toUUIDString(high, low);
        increment();
    }
    index = 0;
}

std::string UUIDFactory::next() {
    if (index >= BATCH_SIZE) {
        generateBatch();
    }
    return batch[index++];
}

std::vector<std::string> UUIDFactory::getBatch(size_t size) {
    std::vector<std::string> result;
    result.reserve(size);
    for (size_t i = 0; i < size; i++) {
        result.push_back(next());
    }
    return result;
}