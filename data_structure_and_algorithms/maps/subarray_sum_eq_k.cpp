// the key idea is that a sliding window does NOT work here. Breaks down with negative values
// instead, the concept of prefix sum is useful here
// define prefix sum as prefix(i) = sum up to i
// then, we want subarray_sum(i, j) = prefix(j) - prefix(i-1). Want this value... inclusive.
// we need total # subarray_sum(i, j) == k.
// how do we do this?
// assume we have prefix(j) = sum to current num.
// then what we want is to know how many prefix(i-1) exist for prefix(j) - k
// note that prefix(i-1) is just any earlier sums
// if we keep a count of prefix's, then sum when we get a match, we get the solution

class Solution {
public:
    int subarraySum(vector<int>& nums, int k) {
        // key is some prefix(i-1) sum, value is count which it has been seen
        // this way, one prefix(j) can match us to many prev prefix(i-1)
        std::unordered_map<int, int> prefix_count;
        // start with prefix count of 0, seen once (at the beginning)
        prefix_count[0] = 1;
        int sum = 0;
        int count = 0;

        for (const auto& num: nums)
        {
            sum += num;
            if (prefix_count.contains(sum-k))
            {
                count += prefix_count[sum-k];
            }

            prefix_count[sum] += 1;
        }
        
        return count;
    }
};