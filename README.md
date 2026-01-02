# BareBones-HLL (HyperLogLog)

A compact Java implementation of the HyperLogLog cardinality estimation algorithm for counting unique elements in large datasets with minimal memory usage.

## Overview

HyperLogLog is a probabilistic algorithm that estimates the cardinality (number of unique elements) of a dataset using significantly less memory than exact counting methods. This implementation is based on the original paper:

**"HyperLogLog: the analysis of a near-optimal cardinality estimation algorithm"**  
Philippe Flajolet, Éric Fusy, Olivier Gandouet, Frédéric Meunier  
https://algo.inria.fr/flajolet/Publications/FlFuGaMe07.pdf

## Features

- **Memory efficient**: Uses only `r × 2^p / 8` bytes
- **Configurable precision**: Supports p=[5,30] and r={4,5,6} bits per register
- **Mergeable**: Combine multiple HLL sketches with union operation
- **Serializable**: Compact binary format for storage and transmission
- **Standard error**: ~1.04/√m where m = 2^p registers

## Usage

```java
import net.openhft.hashing.LongHashFunction;

// Create HLL with p=12 (4096 registers) and r=6 bits per register
HLL hll = new HLL(12, 6);

// Hash and add elements
LongHashFunction hash = LongHashFunction.xx();
hll.add(hash.hashInt(42));
hll.add(hash.hashLong(12345L));
hll.add(hash.hashBytes("hello".getBytes()));

// Get cardinality estimate
long estimate = hll.estimate();

// Merge multiple HLLs
HLL hll1 = new HLL(12, 6);
HLL hll2 = new HLL(12, 6);
// ... add elements to both ...
hll1.merge(hll2);  // hll1 now contains union of both sets

// Serialize/deserialize
byte[] serialized = hll.serialize();
HLL restored = HLL.deserialize(serialized);
```

## Parameters

### p (precision parameter)
- **Range**: 5 to 30
- **Registers**: m = 2^p
- **Higher p** = more accuracy, more memory
- **Recommended**: p=12 for most use cases (4096 registers, ~1.625% error)

### r (register width)
- **Options**: 4, 5, or 6 bits
- **Range**: Each register can store values 0 to 2^r - 1
- **Trade-off**:
    - r=4: Minimal memory, saturates at ~16 leading zeros
    - r=5: Good balance for most datasets
    - r=6: Best for very large cardinalities (billions+)

## Memory Usage

| p  | r | Memory | Registers | Std Error |
| -- | - | ------ | --------- | --------- |
| 10 | 4 | 512 B  | 1,024     | 3.25%     |
| 12 | 4 | 2 KB   | 4,096     | 1.625%    |
| 10 | 5 | 640 B  | 1,280     | 2.6%      |
| 12 | 5 | 2.5 KB | 5,120     | 1.3%      |
| 12 | 6 | 3 KB   | 4,096     | 1.625%    |
| 14 | 6 | 12 KB  | 16,384    | 0.8125%   |

Formula: `memory = r × 2^p / 8` bytes

## Serialization Format

The serialized format is a compact binary representation:

```
[Register Data] [p] [r]
```

- **Bytes 0 to m-1**: Raw register data (bit-packed)
- **Byte m**: Precision parameter (p)
- **Byte m+1**: Register width (r)

Total size: `m + 2` bytes where `m = r × 2^p / 8`

### Example
For p=12, r=6:
- Register data: 3,072 bytes (4,096 registers × 6 bits / 8)
- Metadata: 2 bytes
- **Total**: 3,074 bytes

The register data is bit-packed, meaning registers don't align to byte boundaries. The `readRegister()` and `writeRegister()` methods handle bit-level access transparently.

## Error Characteristics

The standard error is approximately **1.04/√m**:
- This is the expected standard deviation
- ~68% of estimates fall within ±1σ of true value
- ~95% of estimates fall within ±2σ of true value
- Actual error on any single estimate may be higher or lower

## References

Flajolet, P., Fusy, É., Gandouet, O., & Meunier, F. (2007). *HyperLogLog: the analysis of a near-optimal cardinality estimation algorithm*. In AofA: Analysis of Algorithms (pp. 137-156).