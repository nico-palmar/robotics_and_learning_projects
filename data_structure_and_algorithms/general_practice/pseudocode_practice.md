
### 1. Best time to buy and sell stocks

```
procedure maxProfit(v[1...n]):
    low = v[1]
    max_profit = 0

    for item in v:
        low = min(low, item)
        profit = max(max_profit, item - low)
    
    return profit 

```

### 2. Minimum number of frogs croaking
```
// need to keep track of multiple croak instances
// note that each of the letters in croak are unique (so we can easily identify where they should go)
// need to keep an instance vector.. or a "map" which shows where we are

// if you think of this as a state machine c -> r -> o -> a -> k
// each letter (a state) can keep a count of the number of items at the state
// when a new letter comes up, need to check if you can transition from the previous state
// or if it's just a c (and you can start a new instance)

// this can probably be kept in a dictionary with a count, and incremented accordingly

// idea:
// failure: we need different frogs...
procedure minNumberOfFrogs(croakOfFrogs):
    tracker = makeHashMap( { c: 0, r: 0, ...}) -> no need for tracker k
    count = 0
    max_frogs = 0
    for (letter in croackOfFrogs)
        switch (letter):
            letter is c: tracker[c]++; count++; max_frogs = max(count, max_frogs)
            letter is r: 
                if (tracker[c] > 0):
                    tracker[c]--
                    tracker[r]++
                else:
                    return -1
            letter is o:
                if (tracker[r] > 0):
                    tracker[r]--
                    tracker[o]++
                else:
                    return -1
            ...
            letter is k:
                if (tracker[a] > 0):
                    tracker[a]--;
                    count--;
                else:
                    return -1
    
    // check for any unfinished croaks
    if any element in tracker: return -1

    return max_frogs
```


### 3. Basic Calculator II

// string with expression, eval the expression
// key idea: BEDMAS, assume no brackets
// just have integers, and operators with spaces
// want to strip the spaces
// then apply ops in DMAS order (be careful with 0 division?)

// idea: find most important operands, apply them in order
// then move to next operands, apply them, until no operands left

```
// assumption: finding operators from the front in terms of priority is what we want
procedure calculate(string s):

    if (s is empty) return 0

    while(s.length() >= 1):
        s.strip_spaces();
        if (div = s.find("/")):
            a = s[div-1]
            b = s[div+1]
            res = floor(a/b)
            
        else if (mult = s.find("*")):
            a = s[mult-1]
            b = s[mult+1]
            res = a*b
        
        else if (add = s.find("+")):
            a = s[add-1]
            b = s[add+1]
            res = a+b

        else if (sub = s.find("-")):
            a = s[sub-1]
            b = s[sub+1]
            res = a-b
        
        else:
            throw invalid character found
        
        s[div-1] = " "
        s[div+1] = " "
        s[div] = res
    
    return string_to_int(s)
```

// note that above isn't really efficient... even if it "might" work O(n^2)

// the correct idea is using a stack
// should read from left to right
// if it's a + add a number to the stack
// if it's a minus add -number to the stack
// if there's a mult, pop the last from the stack and mult, then push
// if there's a div, pop the last from stack and floor div, then push

// finally, sum everything in the stack (order doesn't matter, can just pop and sum)

```
procedure calculate(string s):
    op = "+"
    stack = []
    // must accumulate the numbers
    curr_ch = ""


    for (ch in s):
        if (ch not operator):
            curr_ch.add_back(ch)
            if (ch not last item):
                continue
        
        if op is +:
            stack.push(int(curr_ch))
            continue
        if op is -:
            stack.push(-int(curr_ch))
            continue
        if op is /:
            a = stack.pop()
            stack.push(floor(a / int(curr_ch)))
        if op is *:
            a = stack.pop()
            stack.push(a * int(curr_ch))
        
        op = ch
        curr_ch = ""
    
    return sum(stack)
```


### 4. H-index

// what are the important concepts to calculate H-index?
// the maximum value of h-index = total number of papers published
// should keep track of papers cited a least x times
// [4, 3, 5, 6, 7, 1]
// paper cited at least 0 times: 6 -> H0
// papers cited at least 1x: 6 -> h1
// paper cited at least 2x: 5 -> h2
// papers cited at least 3x: 5 -> h3
// papers cited at least 4x: 4 -> h4
// it's sort of an accumulation, because papers cited at least 4 times = papers cited at least 3 times - papers cited 3x

// look at [3, 0, 6, 1, 5] -> keep vector where idx corresponds to citation amount
// NO: That is AT MOST.
// [1, 2 (1 + 1 from before), 2 (0 + 2 before), 3 (1 + 2), 3, 4 (1 + 4)]
// better yet... we care about at least. Which means, we accumulate in reverse
// also only need to count up to the number of papers, because everything after isn't possible
// [1, 1, 0, 1, 0, 2] => [5, 4, 3, 3, 2, 2]. When the idx <= count, from the back, then that's the h-index

```
procedure hIndex(v[1...n]):
    if n == 0:
        return 0
    
    for item in v:
        if (item < v.size()):
            count[item]++
        else:
            count[n]++

    accum_val = 0
    for citation_amt, count in count.reverse():
        accum_val += count

        if (accum_val >= citation_amt):
            // this is the h index
            return citation_amt
    return 0
```


### 5. 4Sum

// 4 sum is just adding an extra loop over 3 sum

```
procedure 4sum(v[1..n], target):
    ret
    sort(v)

    for i in range(0, n-3):
        // check if equal to prev
        if i > 0 and v[i] == v[i-1]:
            continue;
        for j in range (i+1, n-2):
            if j > i+1 and v[j] == v[j-1]: continue
            l = j+1
            r = n-1

            while l < r:
                total = sum(i,j,l,r indices)

                if total == target:
                    add (i, j, l ,r) to ret
                    skip l repeats ++
                    skip r repeats --
                
                else if total > target:
                    r--
                else
                    l++
    return ret

```

### 6. Rotting Oranges
// this is a BFS problem
// need to keep track of when we complete one 'level of bfs' = 1 minute
// once the queue is empty, we need to check for any fresh oranges in the graph

// how do we keep track of one level complete? The queue for BFS will need to contain both the (vertex, time) pair, so that when popping/pushing new nodes we can increment the time (and record the last time)

```
procedure orangesRotting(G):
    // initalize rotting oranges
    Q = []
    min_time = 0
    for v element of G:
        if (v == 2):
            // push the rotten vertex, with it's time
            Q.push((v, t=0))
    
    while (!Q.empty()):
        v, t = Q.pop() // gives us next vertex to rot others
        min_time = max(min_time, t) // update our minimum time with the now rotten orange
        for u fresh neighbour of v:
            t_next = t + 1
            u.val = 2 // mark as rotten
            Q.push((u, t_next)) // push the neighbour rotting at the next time step
    
    if fresh orange exists in G: return -1

    return min_time
```

### 7. Fibonnaci Numbers

### 8. Design Circular Queue

// how to design a circular queue?

```
// main thing for a circular queue is that we keep a pointer to the front, and to the end
// both front and end can wrap around using modulo operation

// the only really 'tricky' thing here is that we resize the queue to hold 1 extra spot
// this is so that we can distinguish between full and empty
// we do +1 checks on back, to account for the extra space

class MyCircularQueue {
public:
    MyCircularQueue(int k) {
        q_.resize(k+1);
    }
    
    bool enQueue(int value) {
        // must check if we have space to enqueue to the back
        if (isFull())
        {
            // the back will catch up to the front, no space
            return false;
        }
        // wrap around back to the front
        q_[back_] = value;
        back_ = (back_ + 1) % q_.capacity();
        return true;
    }
    
    bool deQueue() {
        if (isEmpty())
        {
            return false;
        }
        front_ = (front_ + 1) % q_.capacity();
        return true;
    }
    
    int Front() {
        if (isEmpty())
        {
            return -1;
        }
        return q_[front_];
    }
    
    int Rear() {
        if (isEmpty())
        {
            return -1;
        }
        auto idx = (back_ == 0)? q_.capacity() - 1 : back_ - 1;
        return q_[idx];
    }
    
    bool isEmpty() {
        return front_ == back_;
    }
    
    bool isFull() {
        return ((back_+1)%q_.capacity() == front_);
    }
private:
    std::vector<int> q_;
    int front_ { 0 };
    int back_ { 0 };
};

/**
 * Your MyCircularQueue object will be instantiated and called as such:
 * MyCircularQueue* obj = new MyCircularQueue(k);
 * bool param_1 = obj->enQueue(value);
 * bool param_2 = obj->deQueue();
 * int param_3 = obj->Front();
 * int param_4 = obj->Rear();
 * bool param_5 = obj->isEmpty();
 * bool param_6 = obj->isFull();
 */
```

### 9. String Compression