#include "SketchTestRunner.h"

#include <chrono>
#include <iostream>
#include <random>
#include <cmath>

using clock_ns = std::chrono::steady_clock;

static uint64_t nowNs() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        clock_ns::now().time_since_epoch()).count();
}

SketchTestRunner::SketchTestRunner(
    const Config& cfg,
    const SketchType& type,
    int s)
    : config(cfg), sketchType(type), size(s) {}

SketchTestResult SketchTestRunner::runTest() {
    uint64_t startTimeNs = nowNs();

    std::cerr << "    Creating sketch array...\n";
    auto sketches = createSketchArray();

    std::cerr << "    Testing size...\n";
    auto sizeResult = testMinMaxSerializedSize(sketches);

    std::cerr << "    Testing update...\n";
    auto updateResult = testUpdate(sketches);

    std::cerr << "    Testing merge...\n";
    auto mergeResult = testMerge(sketches);

    std::cerr << "    Testing serialize...\n";
    auto serResult = testSerialize(sketches);

    std::cerr << "    Testing deserialize...\n";
    auto deserResult = testDeserialize(sketches);

    std::cerr << "    Testing cardinality...\n";
    auto cardResult = testCardinality(sketches);

    uint64_t endTimeNs = nowNs();

    return SketchTestResult(
        sketchType.name,
        size,
        config.TEST_ITERATIONS,
        config.LGK,
        endTimeNs - startTimeNs,
        updateResult,
        mergeResult,
        serResult,
        deserResult,
        sizeResult,
        cardResult,
        std::vector<MemorySnapshot>(),
        std::vector<EventTags>()
    );
}

std::vector<std::unique_ptr<SketchBase>> SketchTestRunner::createSketchArray() {
    std::vector<std::unique_ptr<SketchBase>> sketches;
    sketches.reserve(config.TEST_ITERATIONS);

    UUIDFactory uuidFactory;
    RandomFactory randomFactory;

    auto randoms = randomFactory.batchDouble(config.TEST_ITERATIONS);

    uint64_t start = nowNs();

    for (int i = 0; i < config.TEST_ITERATIONS; i++) {
        auto sketch = sketchType.factory(config.LGK);

        auto uuids = uuidFactory.getBatch(size);
        for (int j = 0; j < size; j++) {
            sketch->update(uuids[j]);
        }

        if (i > 0 && randoms[i] <= (20.0 / config.TEST_ITERATIONS)) {
            double progress =
                (double)i / config.TEST_ITERATIONS * 100.0;
            std::cerr << "      Created "
                      << progress << "% sketches...\n";
        }

        sketches.push_back(std::move(sketch));
    }

    // eventTags.emplace_back(
    //     "SketchCreation", true, start, nowNs());

    return sketches;
}

SizeResult SketchTestRunner::testMinMaxSerializedSize(
    const std::vector<std::unique_ptr<SketchBase>>& sketches) {

    uint64_t start = nowNs();

    auto firstSize = sketches[0]->serialize().size();
    uint64_t min = firstSize;
    uint64_t max = firstSize;

    for (const auto& s : sketches) {
        auto sz = s->serialize().size();
        min = std::min(min, (uint64_t)sz);
        max = std::max(max, (uint64_t)sz);
    }

    uint64_t end = nowNs();
    return SizeResult(start, end, min, max);
}

OpResult SketchTestRunner::testUpdate(
    std::vector<std::unique_ptr<SketchBase>>& sketches) {

    UUIDFactory factory;
    auto uuids = factory.getBatch(sketches.size());

    uint64_t start = nowNs();
    for (size_t i = 0; i < sketches.size(); i++) {
        sketches[i]->update(uuids[i]);
    }
    uint64_t end = nowNs();

    return OpResult(start, end, (end - start) / sketches.size());
}

OpResult SketchTestRunner::testMerge(
    const std::vector<std::unique_ptr<SketchBase>>& sketches) {

    auto merged = sketchType.factory(config.LGK);

    uint64_t start = nowNs();
    for (const auto& s : sketches) {
        merged->merge(*s);
    }
    uint64_t end = nowNs();

    return OpResult(start, end, (end - start) / sketches.size());
}

OpResult SketchTestRunner::testSerialize(
    const std::vector<std::unique_ptr<SketchBase>>& sketches) {

    std::vector<std::vector<uint8_t>> blobs(sketches.size());

    uint64_t start = nowNs();
    for (int i=0; i<sketches.size(); i++) {
        blobs[i] = sketches[i]->serialize();
    }

    uint64_t end = nowNs();
    return OpResult(start, end, (end - start) / sketches.size());
}

OpResult SketchTestRunner::testDeserialize(
    const std::vector<std::unique_ptr<SketchBase>>& sketches) {

    std::vector<std::vector<uint8_t>> blobs;
    blobs.reserve(sketches.size());

    for (const auto& s : sketches) {
        blobs.push_back(s->serialize());
    }

    uint64_t start = nowNs();
    for (auto& b : blobs) {
        auto tmp = sketchType.factory(config.LGK);
        tmp->deserialize(b);
    }
    uint64_t end = nowNs();

    return OpResult(start, end, (end - start) / blobs.size());
}

CardinalityResult SketchTestRunner::testCardinality(
    const std::vector<std::unique_ptr<SketchBase>>& sketches) {

    uint64_t start = nowNs();

    int totalEstimate = 0;
    for (const auto& s : sketches) {
        totalEstimate += s->cardinality();
    }

    uint64_t end = nowNs();

    // eventTags.emplace_back(
    //     "CardinalityEst", true, start, end);

    int avgEstimate = totalEstimate / sketches.size();
    int actual = size + 1;

    double precision =
        std::abs(avgEstimate - actual)
        / (double) actual * 100.0;

    return CardinalityResult(
        (end - start) / sketches.size(),
        precision,
        avgEstimate,
        actual
    );
}