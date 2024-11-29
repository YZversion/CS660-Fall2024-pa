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
        switch (op) {
            case PredicateOp::GT:
            case PredicateOp::GE:
            case PredicateOp::NE:
                return totalValues;
            case PredicateOp::LT:
            case PredicateOp::LE:
            case PredicateOp::EQ:
                return 0;
        }
    }

    if (v > max) {
        switch (op) {
            case PredicateOp::LT:
            case PredicateOp::LE:
            case PredicateOp::NE:
                return totalValues;
            case PredicateOp::GT:
            case PredicateOp::GE:
            case PredicateOp::EQ:
                return 0;
        }
    }

    int bucketIndex = (v - min) / bw;
    int vInBucketIndex = (v - min) % bw;
    if (bucketIndex < 0) {
        bucketIndex = 0;
    } else if (bucketIndex >= buckets) {
        bucketIndex = buckets - 1;
    }

    switch (op) {
        case PredicateOp::EQ: {
            return histogram[bucketIndex] / bw;
        }
        case PredicateOp::NE: {
            return totalValues - (histogram[bucketIndex] / bw);
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
            ans += histogram[bucketIndex] * (bw - vInBucketIndex - 1) / bw;
            for (int i = bucketIndex + 1; i < buckets; i++) {
                ans += histogram[i];
            }
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
    }
    return 0;
}

