#pragma once

#include "../SketchBase.h"
#include <vector>
#include <string>
#include <cstdint>

// hash function – use xxhash / wyhash / farmhash, etc.
#include <xxhash.h>   // or your preferred hash
#include "HLLPlusPlus.h"

class BareBonesHLL : public SketchBase {
    HLLPlusPlus* sketch;

    static uint64_t hash(const std::string& s) {
        return XXH64(s.data(), s.size(), 0);
    }

public:
    BareBonesHLL(int p, int r) {
        sketch = new HLLPlusPlus(p, r);
    }

    // Add destructor
    ~BareBonesHLL() override {
        delete sketch;
    }

    // Prevent copying (or implement deep copy)
    BareBonesHLL(const BareBonesHLL&) = delete;
    BareBonesHLL& operator=(const BareBonesHLL&) = delete;

    void update(const std::string& value) override {
        sketch->add(hash(value));
    }

    void merge(const SketchBase& other) override {
        const auto& o = dynamic_cast<const BareBonesHLL&>(other);
        sketch->merge(o.sketch);
    }

    std::vector<uint8_t> serialize() override {
        return sketch->serialize();
    }

    void deserialize(const std::vector<uint8_t>& bytes) override {
        delete sketch;  // Free old sketch first!
        sketch = HLLPlusPlus::deserialize(bytes);
    }

    uint64_t cardinality() const override {
        return sketch->estimate();
    }
};

class BareBonesHLL4 : public BareBonesHLL {
public:
    explicit BareBonesHLL4(int p) : BareBonesHLL(p, 4) {}
};

class BareBonesHLL5 : public BareBonesHLL {
public:
    explicit BareBonesHLL5(int p) : BareBonesHLL(p, 5) {}
};

class BareBonesHLL6 : public BareBonesHLL {
public:
    explicit BareBonesHLL6(int p) : BareBonesHLL(p, 6) {}
};