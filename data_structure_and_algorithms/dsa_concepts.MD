## DSA Conceptual Review


### 1. Arrays and Strings

#### A. Two-pointer pattern

Use 2 indicies to iterate through data in a single pass. Makes O(n^2) -> O(nlogn) if you need to sort first or even O(n).

Commonly used in finding pairs/triplets that meet a condition (sum = target), removing duplicates, comparing subsequences

eg: Find if there's a pairwise sum between 2 nums that equals the target
- Naive approach is just double for loop and check indicies
- Two-pointer approach involves sorting the list, then making use of sorted order to move pointers, results in O(nlogn) for sort and only O(n) for iteration => O(nlogn).

#### B. Sliding window

Maintains a subset (window) that slides over an array/string. Used when results of the subarray need to be computed.

Idea: Keep track of what enters/leaves the window as it's moved. This can be done with 2 pointers, hash maps, or queues. 

#### C. Prefix sums

Prefix sums precomputes cumulative totals, to allow O(1) queries over ranges. Generalizes to sums, counts, XORs, or differences.

```
std::vector<int> prefixSum(const std::vector<int>& nums)
{
    if (nums.empty)
    {
        return nums;
    }
    // assume we want the prefix sum, inclusive of the last element
    std::vector<int> prefix_sum(nums.size(), 0);
    prefix_sum[0] = nums[0];

    for (std::size_t i = 1; i < nums.size(); i++)
    {
        prefix_sum[i] = nums[i] + prefix_sum[i-1];
    }
    return prefix_sum;
}
```

#### D. Sorting/Partitioning

Sorting arranges elements based on order; partitioning divides them based on a condition (ie. split based on a condition).

#### E. In-place Ops

Modify the data structure without using any extra space. Could be for reversing an array, rotating an array, or in-place duplicate removal. 

#### F. String Basics

Should consider practicing a few problems in this rhealm later. Consider the operations: concatenating (+ or append()), substring (using substr(pos, len)), search (find()).

Also some common algos like reversing words/sentences, palindromes, or anagram/frequency (did that in the longestSubstring example).


### 2. Hash Tables and Hash Sets

Hash tables store key-value pairs using a hash function, mapping key to index (or bucket). Goal is O(1) lookup, deletions, and insertion. 

In C++, we have `std::unordered_map<Key, value>` or `std::unordered_set<Key>`.

Note that the hash function can have collisions, which C++ `unordered_map` handles with chaining (a linked list of values at each hashed index where there is a collision).

#### A. Frequency Maps
Used in problems involving counting... character frequencies/element frequencies.

```
std::unordered_map<char, int> char_count;
for (const auto& c: s)
{
    char_count[c]++;
}
```

#### B. Grouping w Hash Maps
Hash maps make it easy to group data

```
// group all anagrams in sorted fashion
std::unordered_map<std::string, std::vector<std::string>> anagram_group;
for (const auto& word: words)
{
    auto key = word;
    std::sort(key.begin(), key.end());
    anagram_group[key].push_back(word);
}
```


#### C. Existance checking
For checking existance of an element, an unordered set may be preferred; O(1) search


#### D. Sliding window

Use a hashmap to keep counts of object inside a sliding window.