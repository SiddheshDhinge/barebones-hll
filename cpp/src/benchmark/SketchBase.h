#pragma once
#include <string>
#include <vector>
#include <cstdint>

class SketchBase {
public:
    virtual ~SketchBase() = default;

    virtual void update(const std::string& value) = 0;
    virtual void merge(const SketchBase& other) = 0;

    virtual std::vector<uint8_t> serialize() = 0;
    virtual void deserialize(const std::vector<uint8_t>& bytes) = 0;

    virtual uint64_t cardinality() const = 0;
};