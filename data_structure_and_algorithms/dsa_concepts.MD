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


### 3. Stacks and Queues


#### A. Stack (LIFO)

Stacks are last in, first out data structures => last element pushed is first popped.

In C++...
```
std::stack<T>
push(x) to add element
pop() to remove top element
top () access top
empty(), size()
```
When to use? Function call stack (recursion), undo/redo systems, balanced parenthesis, reverse elements


#### B. Monotomic Stacks

A stack that retains elements in monotonic order. Each time you push a new element, pop from stack to maintain order.

Can use for next greatest element, stock span/temp problems, or histogram largest rectangle

For next greatest element... [3, 2, 5, 1, 4, 3] => [5, 5, -1, 4, -1, -1].

Note that the stack always holds element in monotomically decreasing order.

```
// idea: keep popping off a stack while nums[stack.top()] < curr
for (int i = 0; i < nums.size(); i++)
{
    while (!stack.empty() && nums[i] > nums[stack.top()])
    {
        // the current value is next greatest, pop off
        res[stack.top()] = nums[i];
        stack.pop();
    }
    // signal that we haven't found the next greatest for idx i
    stack.push(i);
}
```

#### C. Queue (FIFO)

First in first out

In C++
```
std::queue<T>
push(x) enqueue (add to back to queue)
pop() to dequeue (remove from front)
front() peek the front
empty(), size()
```

Use cases: BFS, producer consumer, low level traversal in trees

#### D. BFS with Queues
Idea: Process the nodes in order, using a queue
```
void bfs(int start, vector<vector<int>>& graph)
{
    vector dist(graph.size(), math.inf);
    queue<int> q;
    // put the start node in the queue
    q.push(start);
    dist[start] = 0;
    while (!q.empty())
    {
        int next_node = q.front();
        q.pop();
        for (const auto& neighbour: graph[next_node])
        {
            if (dist[neighbour] == math.inf)
            {
                dist[neighbour] = dist[next_node] + 1;
                q.push(neighbour);
            }
        }
    }
}
```

#### E. Dequeue (double ended queue)

Dequeue supports insertion and deletion on both ends
```
C++ implementation:
std::dequeue<T>
push_back(x), push_front(x)
pop_back(), pop_front()
front(), back()
```

Use cases: Sliding window maximum (maximum in decreasing order), implementing stack/queue, palindrome checking (compare ends)

#### F. Sliding window maximum (dequeue)

For sliding window of size k, output max element.
Idea: Maintain monotonic decreasing dequeue of indicies.
That is, max element index is at the front, min element index is at the back

Then for each element O(N)...
1. Pop from the front if element is outside window (can't select old value)
2. Pop from the back if the new element is larger (those can't be the greatest)
3. Push new element at back (larger, acceptable elements are at the front)
4. Largest acceptable is the element at the front

```
vector<int> maxSlidingWindow(vector<int>& nums, int k)
{
    dequeue<int> dq;
    vector<int> res;
    for (int i = 0; i < nums.size(); i++)
    {
        // input is [0, 1, 2, 3]
        // if idx = 3, window is [1, 2, 3] with k = 3. So dist is idx > (i - k + 1)
        // pop front front while out of range
        while (!dq.empty() && dq.front() < (i - k + 1))
        {
            dq.pop_front();
        }

        while (!dq.empty() && nums[dq.back()] < nums[i])
        {
            dq.pop_back();
        }

        dq.push_back(nums[i]);

        // don't add for any of the first few vals
        if (i >= (k - 1))
        {
            res.push_back(nums[dq.front()]);
        }
    }
    return res;
}
```

### 4. Linked Lists

#### A. Data Structure

Linear data structure, where elements (nodes) are connected by pointers. Not contiguous in memory; dynamic size with easy insertion/deletion. Cons: No random access, higher memory overhead.

Node definition:
```
struct Node
{
    int val { 0 };
    Node* next { nullptr };
    Node(int v): val(v);
}
```

#### B. Reversal

Reverse a linked list both recursively or iteratively:

First, iterative
```
Node* reverseList(Node* head)
{
    Node* curr = head;
    Node* prev = nullptr;
    while (curr != nullptr)
    {
        Node* next = curr->next;
        curr->next = prev;
        curr = next;
        prev = curr;
    }
    // end up with curr as new head
    return prev;
}
```

Then recursive
```
Node* reverseListRec(Node* head)
{
    // base case: it's the last node
    if (!head || !head->next)
    {
        return head;
    }
    auto solution_head = reverseListRec(head->next);
    // link the solution to this node
    head->next->next = head;
    head->next = nullptr;
    return solution_head;
}
```

#### C. Merge Two Sorted Lists

Similar to the merge step in merge sort, sort from largest to smallest
```
Node* mergeTwoSortedLists(Node* l1, Node* l2)
{
    if (!l1) return l2;
    if (!l2) return l1;

    if (l1->val > l2->val)
    {
        auto next_head = mergeTwoSortedLists(l1->next, l2);
        l1->next = next_head;
        return l1;
    }
    else
    {
        auto next_head = mergeTwoSortedLists(l1, l2->next);
        l2->next = next_head;
        return l2;
    }
}
```


#### D. Fast/Slow Pointers

Used for finding middle and cycle detection.

Can use slow/fast ptrs. Slow up by 1, fast up by 2.

Finding middle: when fast reaches the end, then slow is in the middle.

```
Node* middleNode(Node* head)
{
    auto fast, slow = head;
    while (fast && fast->next)
    {
        fast = fast->next->next;
        slow = slow->next;
    }
    return slow;
}
```

Cycle detection and floyd's algorithm:

```
bool hasCycle(Node* head)
{
    auto slow, fast = head;
    while (fast && fast->next)
    {
        fast = fast->next->next;
        slow = slow->next;

        if (fast == slow)
        {
            return true;
        }
    }
    return false;
}
```


#### E. Recursion + Iteration

Recursion: Cleaner
Iteration: More verbose

Recursion: O(n) space complexity, stack
Iteration: O(1) space complexity

Recursion: Can be slower due to function calls
Iteration: Might be faster

Recursion: Good when conceptually recursive
Iteration: For large linked lists, avoids stack overflow.