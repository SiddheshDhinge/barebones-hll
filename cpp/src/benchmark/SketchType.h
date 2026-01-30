#pragma once
#include <string>
#include <functional>
#include <memory>
#include <vector>
#include "SketchBase.h"

struct SketchType {
    std::string name;
    std::function<std::unique_ptr<SketchBase>(int)> factory;
};

// Registry
inline std::vector<SketchType>& sketchRegistry() {
    static std::vector<SketchType> registry;
    return registry;
}

// Helper macro to register sketches
#define REGISTER_SKETCH(name, cls) \
    sketchRegistry().push_back({ name, [](int lgK) { return std::make_unique<cls>(lgK); } })