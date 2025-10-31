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



### 5. Trees

#### A. Basics

Trees are a connected, acyclic structure. Root is top-most node. Each node has 0 or more children. Binary trees have only 2 children. Binary Search Tree (BST) have property `left.val < root.val < right.val`.

Common Node Definition:
```
struct TreeNode
{
    int val;
    TreeNode* left { nullptr };
    TreeNode* right { nullptr };
    TreeNode(int v): val(v) {}
};
```


#### B. BFS and DFS

DFS goes deep first. Uses a stack or recursion. Commonly used for Traversals, height, and LCA (lowest common ancestor, I think). 

BFS goes level by level. Uses a queue. Commonly used for level order traversal, or shortest path in an unweighted tree (similar to BFS in graphs).

Note that for binary trees, there are 3 common traversals for DFS (in order, pre order, post order). Of course, there is also level order (BFS style).

Level order traversal: Use queue

```
void bfs(TreeNode* root)
{
    if (!root) return;
    std::queue<TreeNode*> q;
    q.push(root);
    while (!q.empty())
    {
        auto curr = q.front();
        q.pop();
        // visit the current node, add it's neighbours to the queue
        if (curr->left) q.push(curr->left);
        if (curr->right) q.push(curr->right);
    }
}
```

DFS Inorder traversal: Left -> Root -> Right
```
void inorder(TreeNode* root)
{
    if (!root) return;
    inorder(root->left);
    // visit the root
    inorder(root->right);
}
```

DFS Preorder traversal: Root -> left -> right
```
void preorder(TreeNode* root)
{
    if (!root) return;
    // visit the root
    inorder(root->left);
    inorder(root->right);
}
```

DFS Postorder: Left -> right -> root


#### C. Tree Diameter and Height

Height: Length of longest path from root -> leaf node

```
int height(TreeNode* root)
{
    if (!root) return 0;
    return 1 + std::max(height(root->left), height(root->right));
}
```

Diameter: Longest path between 2 nodes (which can pass through the root, but doesn't have to).
```
int dfs(TreeNode* root, int& max_d)
{
    if (!root) return 0;
    // concept: use DFS to compute the heights
    // then, keep track of the max_d for parent recursion
    // the function actaully returns the max height (not diameter)
    const auto left_height = dfs(root->left, max_d);
    const auto right_height = dfs(root->right, max_d);
    // global maximum diameter; can get passed by it's children
    max_d = std::max(max_d, left_height + right_height);
    return 1 + (std::max(left_height, right_height));
}

int depth(TreeNode* root)
{
    int max_d = 0;
    dfs(root, max_d);
    return max_d;
}
```


#### D. Lowest Common Ancestor (LCA)

LCA: Given two nodes in a tree; p and q, LCA is the lowest node in the tree that has both p and q as descendants. Note that a node can be a descendant of itself.

Case 1: Generic Binary Tree... use DFS recursion

```
TreeNode* LCA(TreeNode* root, TreeNode* p, TreeNode* q)
{
    // base case: either root is empty
    // or root is p (LCA(p, p, q) -> p)
    // or root is q (LCA(q, p, q) -> q)
    // acknowledge either finding one of them, or finding nothing
    if (!root || root == p || root == q)
    {
        return root;
    }

    // finds if any LCA exists on left
    auto left = LCA(root->left, p, q);
    // finds if any LCA exists on right
    auto right = LCA(root->right, p, q);

    // note that now we are in post order
    // if we got back left and right, this must be the lowest ancestor
    if (left && right)
    {
        return root;
    }
    // otherwise, return the non-empty node
    return left? left: right;
}
```

Case 2: Binary Search Tree
Idea: In a BST, we can use values for comparison.
If both p and q smaller, then LCA is on the left.
If both p and q larger, then on right.
Otherwise, this is LCA.
```
TreeNode* LCA_BST(TreeNode* root, TreeNode* p, TreeNode* q)
{
    if (!root) return root;

    if (root->val > p->val && root->val > q->al)
    {
        return LCA_BST(root->left, p, q);
    }
    else if (root->val < p->val && root->val < q->val)
    {
        return LCA_BST(root->right, p, q);
    }
    // if not, this is the LCA, one larger and other smaller
    return root;
}

```

#### E. Balanced Trees

Balanced binary tree: For every node, diff between left and right <= 1.
Makes the tree height = about O(logn).

Types of balanced trees: AVL Tree, red black tree, b-trees.

The concept is to check for balance and height of the tree at the same time (because if you do it separately, you end up with O(n^2) time complexity; together it's O(n)).

```
bool isBalanced(TreeNode* root, int& height)
{
    if (!root)
    {
        height = 0;
        return true;
    }
    int left_height = 0;
    int right_height = 0;
    bool left_balanced = isBalanced(root, left_height);
    bool right_balanced = isBalanced(root, right_height);

    height = 1 + std::max(left_height, right_height);
    return (left_balanced && right_balanced && std::abs(left_height - right_height) <= 1);
}
```


### 6. Binary Search + Divide and Conquer

#### A. Binary Search

Binary search is O(logn) algorithm that works with sorted data. Iteratively (or recursively) half the search space, based on some condition.

```
int binarySearch(vector<int>& nums, int target)
{
    int l = 0;
    int r = nums.size() - 1;

    while (l < r)
    {
        int mid = l + (r-l)/2;
        auto mid_elem = nums[mid];
        if (mid_elem == target)
        {
            return mid;
        }
        // assume sorted from smallest to largest
        else if (mid_element < target)
        {
            l = mid+1;
        }
        else
        {
            r = mid-1;
        }
    }
    return -1;
}
```


#### B. Lower Bound (First true)

Find smallest index where condition(mid) becomes true

```
int lowerBound(vector<int>& nums, int target)
{
    int l = 0; r = nums.size();
    while (l < r)
    {
        int mid = l + (r - l) / 2;
        if (nums[mid] >= target) r = mid;   // move left
        else l = mid + 1;                   // move right
    }
    return l; // smallest i with nums[i] >= target
}
```
 
#### C. Divide and Conquer... review ECE 406 notes

Some common algorithms...

Merge sort: Split array in half, merge sorted halves
Quick sort: Parition by pivot, combine recursively
Binary tree recursion: Recurse left/right subtrees, combine child results.

Let's go over merge sort and quick sort in a bit more detail...


#### D. Sorting

Can be broken down into 2 steps... 
Outer function finds the middle index of a vector, and calls merge(mergesort(half1), mergesort(half2));

The inner function, merge takes 2 merged arrays, and combines them recursively (puts smaller elem first, until any array is all processed).


For quick sort, you have 2 general functions...
1. Partition: This function randomly selects a pivot element (or chooses the last value). You then go through the entire range of the vector of interest (keeping a store position). If the element is less than the pivot, then swap the current position and store position.. advancing the current position. You then place the pivot (at the end) in the correct spot and return the pivot.
2. An outer quicksort function which partitions the array into 2 (with a pivot) then calls quicksort from both halves of the pivot.


#### E. Middle Finding

Quick select: Find the value that would be at k-position if the array were sorted.
Same idea as quick sort.. us the partition function in 1. The high level wrapper is then different, where if k is pivot you found it. Otherwise, look in either left/right depending on if k < pivot or k > pivot.
