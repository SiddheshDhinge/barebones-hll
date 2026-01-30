#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include "SketchBase.h"
#include "SketchType.h"
#include "ArgParser.h"
#include "Results.h"
#include "UUIDFactory.h"
#include "RandomFactory.h"

class SketchTestRunner {
    const Config& config;
    const SketchType& sketchType;
    int size;

    std::vector<std::unique_ptr<SketchBase>>
    createSketchArray();

    static SizeResult
    testMinMaxSerializedSize(
        const std::vector<std::unique_ptr<SketchBase>>& sketches);

    static OpResult
    testUpdate(std::vector<std::unique_ptr<SketchBase>>& sketches);

    OpResult
    testMerge(const std::vector<std::unique_ptr<SketchBase>>& sketches);

    static OpResult
    testSerialize(
        const std::vector<std::unique_ptr<SketchBase>>& sketches);

    OpResult
    testDeserialize(
        const std::vector<std::unique_ptr<SketchBase>>& sketches);

    CardinalityResult
    testCardinality(
        const std::vector<std::unique_ptr<SketchBase>>& sketches);

public:
    SketchTestRunner(
        const Config& cfg,
        const SketchType& type,
        int size);

    SketchTestResult runTest();
};