#include <db/ColumnStats.hpp>

using namespace db;

ColumnStats::ColumnStats(unsigned buckets, int min, int max)
  : buckets(buckets), min(min), max(max), histogram(buckets, 0), totalValues(0)
// TODO pa4: some code goes here
{
  // TODO pa4: some code goes here
  bucketWidth = static_cast<double>(max - min + 1) / buckets;
}

void ColumnStats::addValue(int v) {
  // TODO pa4: some code goes here
  if (v > max || v < min) {
    // Value out of range, ignore it
    return;
  }
  unsigned bucketIndex = static_cast<unsigned>((v - min) / bucketWidth);
  //bucket index bounded
  bucketIndex = std::min(bucketIndex, buckets - 1);
  histogram[bucketIndex] += 1;
  totalValues += 1;
}

size_t ColumnStats::estimateCardinality(PredicateOp op, int v) const {
  if (v < min) {
    if (op == PredicateOp::GT) {
      return totalValues;
    } else if (op == PredicateOp::LT) {
      return 0;
    } else if (op == PredicateOp::EQ) {
      return 0;
    }
  }
  if (v > max) {
    if (op == PredicateOp::GT) {
      return 0;
    } else if (op == PredicateOp::LT) {
      return totalValues;
    } else if (op == PredicateOp::EQ) {
      return 0;
    }
  }
  // TODO pa4: some code goes here
  unsigned bucketIndex = static_cast<unsigned>((v - min) / bucketWidth);
  bucketIndex = std::min(bucketIndex, buckets - 1); // Ensure the index is within bounds
  double bucketStart = min + bucketIndex * bucketWidth;
  double bucketEnd = bucketStart + bucketWidth;
  int bucketHeight = histogram[bucketIndex];
  size_t estimatedCardinality = 0;

  switch (op) {
  case PredicateOp::EQ: {
    if (bucketWidth == 0) {
      return 0;
    }
    double selectivity = 1.0 / bucketWidth;
    estimatedCardinality = static_cast<size_t>(bucketHeight * selectivity);
    break;
  }
  case PredicateOp::GT: {
    if (bucketWidth == 0) {
      return 0;
    }
    double fractionInBucket = (bucketEnd - v) / bucketWidth;
    estimatedCardinality += static_cast<size_t>(bucketHeight * fractionInBucket);
    for (unsigned i = bucketIndex + 1; i < buckets; ++i) {
      estimatedCardinality += histogram[i];
    }
    break;
  }
  case PredicateOp::LT: {
    if (bucketWidth == 0) {
      return 0;
    }
    double fractionInBucket = (v - bucketStart) / bucketWidth;
    estimatedCardinality += static_cast<size_t>(bucketHeight * fractionInBucket);
    for (unsigned i = 0; i < bucketIndex; ++i) {
      estimatedCardinality += histogram[i];
    }
    break;
  }
  default:
    // Handle other cases as needed
      break;
  }
  return estimatedCardinality;
}
