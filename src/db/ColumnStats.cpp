#include <db/ColumnStats.hpp>

using namespace db;

ColumnStats::ColumnStats(unsigned buckets, int min, int max)
    : buckets(buckets), min(min), max(max), histogram(buckets, 0), totalValues(0)
  // TODO pa4: some code goes here
  {
    // TODO pa4: some code goes here
    bw = (max - min + buckets) / buckets;
}

void ColumnStats::addValue(int v) {
    // TODO pa4: some code goes here
    if (v > max || v < min) {
      // Value out of range, ignore it
      return;
    }
    unsigned bucketIndex = static_cast<unsigned>((v - min) / bw);
    //bucket index bounded
    bucketIndex = std::min(bucketIndex, buckets - 1);
    histogram[bucketIndex] += 1;
    totalValues += 1;
}

size_t ColumnStats::estimateCardinality(PredicateOp op, int v) const {
    if (v < min) {
      if (op == PredicateOp::GT) {
          return totalValues; // All values are greater than v
      } else if (op == PredicateOp::LT) {
          return 0; // No values are less than v
      } else if (op == PredicateOp::EQ || op == PredicateOp::LE || op == PredicateOp::GE) {
          return 0; // No values are equal to, less than or equal, or greater than or equal to v
      } else if (op == PredicateOp::NE) {
          return totalValues; // All values are not equal to v
      }
  }
  if (v > max) {
      if (op == PredicateOp::GT) {
          return 0; // No values are greater than v
      } else if (op == PredicateOp::LT) {
          return totalValues; // All values are less than v
      } else if (op == PredicateOp::EQ || op == PredicateOp::LE || op == PredicateOp::GE) {
          return 0; // No values are equal to, less than or equal, or greater than or equal to v
      } else if (op == PredicateOp::NE) {
          return totalValues; // All values are not equal to v
      }
  }

  unsigned bucketIndex = (v - min) / bw;
  int all = 0;
  int vInBucketIndex = (v - min) - (bucketIndex * bw);

  for (int i = 0; i < buckets; i++) {
      all += histogram[i];
  }

  switch (op) {
      case PredicateOp::EQ: {
          return histogram[bucketIndex] / bw;
      }
      case PredicateOp::NE: {
          return all - histogram[bucketIndex] / bw;
      }
      case PredicateOp::LT: {
          int ans = 0;
          for (int i = 0; i < bucketIndex; i++) {
              ans += histogram[i];
          }
          ans += histogram[bucketIndex] * vInBucketIndex / bw;
          return ans;
      }
      case PredicateOp::LE: {
          int ans = 0;
          for (int i = 0; i < bucketIndex; i++) {
              ans += histogram[i];
          }
          ans += histogram[bucketIndex] * (vInBucketIndex + 1) / bw;
          return ans;
      }
      case PredicateOp::GT: {
          int ans = 0;
          for (int i = bucketIndex + 1; i < buckets; i++) {
              ans += histogram[i];
          }
          ans += histogram[bucketIndex] * (bw - vInBucketIndex - 1) / bw;
          return ans;
      }
      case PredicateOp::GE: {
          int ans = 0;
          ans += histogram[bucketIndex] * (bw - vInBucketIndex) / bw;
          for (int i = bucketIndex + 1; i < buckets; i++) {
              ans += histogram[i];
          }
          return ans;
      }
      default:
          break;
  }
  return 0;
}

