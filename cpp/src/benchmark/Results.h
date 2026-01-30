#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct OpResult {
    uint64_t startTimeNs;
    uint64_t endTimeNs;
    uint64_t avgOpTimeNs;

    OpResult() = default;

    OpResult(uint64_t start,
             uint64_t end,
             uint64_t avg)
        : startTimeNs(start),
          endTimeNs(end),
          avgOpTimeNs(avg) {}
};

struct SizeResult {
    uint64_t startTimeNs;
    uint64_t endTimeNs;
    uint64_t minSize;
    uint64_t maxSize;

    SizeResult() = default;

    SizeResult(uint64_t start,
               uint64_t end,
               uint64_t minSz,
               uint64_t maxSz)
        : startTimeNs(start),
          endTimeNs(end),
          minSize(minSz),
          maxSize(maxSz) {}
};

struct CardinalityResult {
    uint64_t time;                 // avg time per op (ns)
    double errorPercentage;
    uint64_t estimatedCardinality;
    uint64_t actualCardinality;

    CardinalityResult() = default;

    CardinalityResult(uint64_t timeNs,
                      double errorPct,
                      uint64_t estimated,
                      uint64_t actual)
        : time(timeNs),
          errorPercentage(errorPct),
          estimatedCardinality(estimated),
          actualCardinality(actual) {}
};

struct MemorySnapshot {
    uint64_t timestamp;
    uint64_t usedHeap;
    uint64_t committedHeap;
    uint64_t maxHeap;

    MemorySnapshot() = default;

    MemorySnapshot(uint64_t ts,
                   uint64_t used,
                   uint64_t committed,
                   uint64_t max)
        : timestamp(ts),
          usedHeap(used),
          committedHeap(committed),
          maxHeap(max) {}
};

struct EventTags {
    uint64_t startTimeNs;
    uint64_t endTimeNs;
    std::string markTag;
    bool isRegion;

    EventTags() = default;

    EventTags(const std::string& tag,
              bool region,
              uint64_t startNs,
              uint64_t endNs)
        : startTimeNs(startNs),
          markTag(tag),
          isRegion(region) {
        endTimeNs = region ? endNs : startNs;
    }
};

struct SketchTestResult {
    std::string sketchType;
    int sketchSize;
    int numSketches;
    int lgK;
    uint64_t timeTakenNs;

    OpResult updateResult;
    OpResult mergeResult;
    OpResult serializeResult;
    OpResult deserializeResult;
    SizeResult sizeResult;
    CardinalityResult cardinalityResult;

    std::vector<MemorySnapshot> memorySnapshots;
    std::vector<EventTags> eventTags;

    SketchTestResult() = default;

    SketchTestResult(const std::string& sketchType_,
                     int sketchSize_,
                     int numSketches_,
                     int lgK_,
                     uint64_t timeTakenNs_,
                     const OpResult& update,
                     const OpResult& merge,
                     const OpResult& serialize,
                     const OpResult& deserialize,
                     const SizeResult& size,
                     const CardinalityResult& cardinality,
                     const std::vector<MemorySnapshot>& memory,
                     const std::vector<EventTags>& events)
        : sketchType(sketchType_),
          sketchSize(sketchSize_),
          numSketches(numSketches_),
          lgK(lgK_),
          timeTakenNs(timeTakenNs_),
          updateResult(update),
          mergeResult(merge),
          serializeResult(serialize),
          deserializeResult(deserialize),
          sizeResult(size),
          cardinalityResult(cardinality),
          memorySnapshots(memory),
          eventTags(events) {}
};