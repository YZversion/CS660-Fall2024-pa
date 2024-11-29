Design Decisions

Switch Case for PredicateOp: I used a switch-case statement to handle different PredicateOp operations (EQ, NE, LT, LE, GT, GE). This allows for clear and structured handling of each predicate, ensuring that every case is dealt with appropriately and preventing unintended behavior for unsupported operations.

Boundary Conditions and Out-of-Range Values: I included explicit checks for values of v that fall outside the range [min, max]. This ensures that we handle out-of-range queries correctly without attempting to access invalid indices in the histogram. This also ensures that all edge cases are properly handled, such as when v is much smaller or larger than the defined range.

Partial Contribution Calculations: For LT, LE, GT, and GE predicates, I first calculated the in-bucket index using modulo arithmetic (vInBucketIndex = (v - min) % bucketLength). This allowed me to determine the exact position of v within its bucket, which is crucial for calculating partial contributions. Depending on whether we were interested in values less than or greater than v, I used either the in-bucket index or (bucketLength - vInBucketIndex - 1) to determine the number of values to add from the current bucket.

Time Spent

I spent approximately 6 hours revising and testing the code. 

Confusion and Challenges

The most challenging part of this programming assignment was dealing with the GT (greater than) and LT (less than) operations, especially when calculating the partial contributions from a bucket. Initially, I found it difficult to determine how to accurately include only the relevant portion of a bucket when v was not at a boundary. However, after experimenting with modulo arithmetic to find the in-bucket index and using different approaches to include the partial contributions, I was able to implement a solution that passed all tests. Once I understood the logic behind the partial bucket contributions, the rest of the implementation became much clearer and easier to complete.