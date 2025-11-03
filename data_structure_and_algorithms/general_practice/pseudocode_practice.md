
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


### 7. Fibonnaci Numbers

### 8. Design Circular Queue

### 9. String Compression