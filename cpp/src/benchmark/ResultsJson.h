#pragma once
#include "Results.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

inline void to_json(json& j, const OpResult& r) {
    j = {
        {"startTimeNs", r.startTimeNs},
        {"endTimeNs", r.endTimeNs},
        {"avgOpTimeNs", r.avgOpTimeNs}
    };
}

inline void to_json(json& j, const SizeResult& r) {
    j = {
        {"startTimeNs", r.startTimeNs},
        {"endTimeNs", r.endTimeNs},
        {"minSize", r.minSize},
        {"maxSize", r.maxSize}
    };
}

inline void to_json(json& j, const CardinalityResult& r) {
    j = {
        {"time", r.time},
        {"errorPercentage", r.errorPercentage},
        {"estimatedCardinality", r.estimatedCardinality},
        {"actualCardinality", r.actualCardinality}
    };
}

inline void to_json(json& j, const EventTags& e) {
    j = {
        {"markTag", e.markTag},
        {"isRegion", e.isRegion},
        {"startTimeNs", e.startTimeNs},
        {"endTimeNs", e.endTimeNs}
    };
}

inline void to_json(json& j, const MemorySnapshot& m) {
    j = {
        {"timeNs", m.timestamp},
        {"usedBytes", m.usedHeap}
    };
}

inline void to_json(json& j, const SketchTestResult& r) {
    j = {
        {"sketchType", r.sketchType},
        {"sketchSize", r.sketchSize},
        {"numSketches", r.numSketches},
        {"lgK", r.lgK},
        {"timeTakenNs", r.timeTakenNs},
        {"updateResult", r.updateResult},
        {"mergeResult", r.mergeResult},
        {"serializeResult", r.serializeResult},
        {"deserializeResult", r.deserializeResult},
        {"sizeResult", r.sizeResult},
        {"cardinalityResult", r.cardinalityResult},
        {"memorySnapshots", r.memorySnapshots},
        {"eventTags", r.eventTags}
    };
}