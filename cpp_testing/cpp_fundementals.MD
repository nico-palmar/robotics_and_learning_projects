# C++ Fundementals Notes

## A. Memory, Pointers, and References

- Stack vs Heap Allocation: 

Stack memory allocation (where stack memory is known ahead of time) is used for local variables, function params, and function calls. It automatically pushes these local variables on the stack, and pops them off the stack at the end (automatically freed). Local scope and limited size. On the other hand, heap memory allocation is done dynamically during program execution; used for large data structures or objects requiring lifetime/global access. In C++, it requires manual deallocation - and can also be slower.


- Smart pointers: 

Smart pointers are used to prevent memory leaks that can be caused by dynamic memory allocation. They manage memory for programmers, often deallocating themselves when they go out of scope (so that a programmer does not need to think about them). 


- Types of smart pointers:
    1. `unique_ptr`: Stores only one pointer at a time. Cannot copy a unique_ptr, only transfer it with move semantics (to transfer it's ownership). Operators: `get()` return raw pointer to managed object. `release()` releases ownership and returns raw pointer. `reset()` destroys currently managed object.
    2. `shared_ptr`: More than one pointer can point to the same object at once; maintain a reference counter.`use_count()` is # of shared pointer instances to same object. `unique()` for single reference. Also the ones for unique_ptr above.
    3. `weak_ptr`: Similar to shared ptr, but will not maintain a reference counter. Used to check for existance of pointers or to prevent circular dependencies from never deallocating. `reset()` clears weak ptr, making it empty. `expired()` returns true if shared pointer instances of object have been destroyed. `lock()` attempts to aquire ownership; if alive returns a shared_ptr that shares ownership (this WILL increase the count from the reference counter); if object dealloc it returns an empty shared pointer. Primary way to interact w object. 


- Ownership Semantics and RAII

Ownership, in C++, refers to who is in charge of managing (allocating/freeing) a resource, typically in memory. The types of ownership closely relates to the types of smart pointers (above).
    1. `unique_ptr`: Only one owner exists; when owner goes out of scope resource is destroyed. Ownership can be transferred with move.
    2. `shared_ptr`: Multiple owners exists; when last owner goes out of scope resource is destroyed.
    3. `weak_ptr`: Observes a shared pointer without owning; can interact with the shared pointer temporarily.
    4. Raw pointer: Just points to resource, no lifetime management.


RAII (Resource Aquisition Is Initialization) refers to a C++ principle that ties the lifetime of a resource to the lifetime of the object. It means to aquire a resource in a constructor, release it in the destructor. This directly relates to how smart pointers operate (think of a unique pointer). In the contructor, it will aquire memory and in the destructor it will free memory (at the end of function scope, rather than having a raw pointer). Also used for locks (mutex), and containers like vectors.


- Dangling pointers and how to avoid


A dangling pointer is a pointer that points to memory that has been freed. Accessing the pointer leads to undefined behaviour. This can be avoided by using smart pointers or STL containers (which handle dynamic memory allocation),  or setting raw pointers to null after deletion. Another weird case to cause dangling pointers is returning a reference/pointer from a local variable - avoid this. Return either by value or safe dynamic memory object.


- const correctness (e.g., const int* vs int* const vs const int* const)

Usage of const is important to prevent changing values that should not be changed. Methods for a function can also be marked as const, if no class attributes are modified in the method.

`const int*`: This refers to a pointer to a constant integer. That is, the pointer can change, but the value it holds is immutable. The pointer may change.
`int* const`: Constant pointer of integer type. This means that the pointer itself cannot be changed (points to the same object in memory), but the object can be modified.
`const int* const`: Constant pointer to constant integer type. Both the value and the pointer are immutable.

- Pass by (value/reference/pointer)

Pass by value => copy of the argument's value is made in memory; changes to the parameter do not affect the passed argument.

Pass by reference => a direct reference is made to the argument, no memory copy; changes to the parameter do affect the argument (which is why we normally use const ref for copy efficiency but ensure immutability).

Pass by pointer => Pass a memory reference (pointer) to the original variable; can be modified by using the pointer. Potentially less safe since the memory address itself may be changed. Useful for accessing dynamically allocated memory (pointers), but otherwise bias against it (for safety).

## B. Object Oriented Programming

- Class vs struct (default access modifiers)

The difference is that classes are private by default, and structs are public. Private means that the members are only accessible within a class itself.. or by friend class/function. Public means that the members are accessible outside the class. Private helps to use the concept of encapsulation, where users of an object interact with designed interfaces, for controlled access to internal state. Structs are often used for more simple data structures, where encapsulation might just add overhead (not necessary).



- Inheritance vs composition (when to use each)

Inheritance => "Is A" relationship. Inheritance is used when derived class is a specialized version of a base class. Eg, car is a vehicle. Benefits include code reuse, by inheriting public and protected members. Also allows for polymorphism, which is crucial for common interfaces.

Composition => "Has A" relationship. Also called containership sometimes. The contained object contributes to the functionality of the container class. It's used when a component is part of the internal structure. Eg. Car has an engine, steering wheel, etc. Benefits: Changes in contained class (thing inside) have less impact on containing class.. which promotes modularity. This means we can easily swap contained objects with different implementations for flexiblity. 

- Virtual functions & dynamic dispatch (vtable)

Virtual functions occur by using the `virtual` keyword in a base class, which signals that a function can be overriden by a derived class, using the keyword `override` (as good pratice, although optional). They allow for dynamic dispatch, or runtime polymorphism where the correct version of the overriden function is chosen at runtime, based on the actual type of object being pointed to, not the type of the pointer. This allows to have a common interface between multiple classes, but uses the correct, desired version at runtime.

How does this work? It uses a vtable which is essentially some function pointers which map to the implenentations of classes that use a virtual function (inherited or derived). Then, a vptr is used to look into the vtable for the correct implementation.

- Abstract classes & pure virtual functions

An abstract class in C++ is a class that cannot be instantiated as a result of having at least one pure virtual function; it's primary purpose is to serve as a base class (common interface) for other derived classes.. used in polymorphism (so you can declare pointers/references to an abstract class for runtime polymorphism). Pure virtual functions are defined with virtual and =0, which just declares the existance of that method in the base class but provides no implementation. This enforces an interface, as derived classes MUST implement this method.


- The “Rule of 3 / 5 / 0”

Pre C++ 11, used rule of 3 to say that any class that might need any of these 3, should define all 3: Destructor, copy constructor, and copy assignment operator. 

Post C++ 11, move semantics added move constructor/assignment operator as well.

Now (modern C++), we have the rule of 0, where we should use the RAII principle (ie. use smart pointers to encapsulate the resource management). In this case, no special methods are needed, and we can rely on compiler generated functions. 

- copy constructor: `ClassName(const ClassName& other);`

A copy constructor is a special type of constructor that creates a new object by copying the member values from an existing object of the same class. By default, it performs a shallow copy, copying the value of member variables directly... which is OK for simple types since they get copied as separate instances. However, for pointers or references, the shallow copy points to the same memory location, meaning that a change in one object would lead to a change in the other. A user defined copy constructor is used when handling classes that deal with dynamic memory, to make deep copies. 

Copy constructors in general get invoked here: Object initialized with another object of same type, object is passed by value to function, obj returned by value from function.

- copy assignment operator: `ClassName& operator=(const ClassName& other);`

Copy the contents of one existing object into another existing object of the same class. Invoked when using `=` with 2 objects of the same class, that have already been constructed. It mainly ensure a deep cope of the resource it's managing. That means, if it has a raw pointer, it must delete it's raw pointer and reallocate the new resources via deep copy. If it's a smart pointer, say a unique pointer, it's just a matter of changing the data in the copy assignment operator (as opposed to creating a new unique pointer in the copy constructor). 

- constructor: `MyClass()`

Automatically called when object of the class is created. If we do not specify, the default constructor (0 arguments) is used. If we do define a constructor, the default constructor is not generated. We use the constructor to initialize attributes in the class.

- destructor: `~MyClass()`

Special member function, automatically invoked when an object of the class is destroyed (out of scope, or delete called on dynamically allocated memory). In charge of resource deallocation (especially dynamic memory), closing file handles, or releasing any other aquired resources.


- move constructor: `ClassName(ClassName&& other);`

Uses `&&` to create rvalue (temp value). It's used to move the source object to the target object, making it more efficient than copying. This is useful for temporary objects, or working with large data (copying large data is expensive). Usage: `std::move(object)` is used to cast lvalue to rvalue reference... so that the move constructor can be selected.

- move assignment operator: `ClassName& operator=(ClassName&& other) noexcept;`

Same ideas as difference between copy assignment operator vs constructor. Efficiently assign contents from one object to another, without making a copy. Usage is 2 constructed objects, then `a = std::move(b)`.

- override, final, and virtual keywords

Override => Used in derived class to explicitly indicate that it is overriding a virtual function from a base class (optional usage; good practice).

Final => Prevents further overriding of a virtual function in a derived class. For a class, it prevents the class from being used further in inheritance. Note: `void foo() final` is equivalent to  `virtual void foo() final` in a derived class. You just don't need to write the virtual in a derived class bc that is implicit. 

Virtual => Used to enable runtime polymorphism; signals to compiler that a derived class my override the method, and that the actual implementation should depend on the actual type of object being pointed to (made clear at runtime).

- Slicing problem

Occurs when object of derived class is assigned or copied to an obj of the base class (by value)... which results in losing derived data members + virtual function overrides. Eg. Assume Derived class is defined. Then `Derived a` `Base b = a`, and b here becomes sliced. To prevent, can use smart pointers to manage dynamically allocated objects and prevent slicing... `Derived a` `unique_ptr<Base> b = std::make_unique<Base>(a)` should work fine. 

## C. Modern C++

- Move semantics (std::move, std::forward)

Move semanitcs are made for efficiency; to allow for efficient transfer of resources by moving them instead of making a deep copy. They use rvalue references (temporary values).

`std::move`: Cast that turns an object into an r-value reference, signaling that it can be "moved from". By being an rvalue reference, it signals for certain things (move constructor/assignment operator) to be used. It is used when returning from functions, storing into containers, or moving smart pointers (ie. cannot copy unique_ptr, only move). Casts any object to rvalue, to then 'steal' it. 

`std::forward`: Makes perfect forwarding possible, preserves value category (lvalue vs rvalue) when passing to another function. lvalue stays lvalue, rvalue stays rvalue. Used in template values to remember original type/value category. `std::forward<\T>(arg)`.. casts arg to the correct value recieved by template function. 

- Rvalue references vs lvalue references

Lvalue reference are objects/functions with an identifiable memory location and persists past the current expression. Name, persistent memory loc, appear on left side of assignment operator. Variables are an example. `int x = 5`, `int& lref = x`.

Rvalue reference are temporary expressions, which do not persist memory location and expire at the end of the full expression in which they appear. Unnamed, no memory location, typically right side of equal sign. Eg. Literals (like 5). `int&& rref = 5`.

- auto, decltype, constexpr, and enum class

`auto`: Allows compiler to deduce variable type from it's initializer

`decltype`: Queries type of an expression, returns the exact type (including const + reference qualifiers)

`constexpr`: Says that a variable or function can be evaluated at compile time, for optimizations

`enum class`: Strongly typed, scoped enum. Prevents implicit conversions, avoid name clashing... safer in general.

- Lambda expressions and captures

Lambda expression lets you define an anonymous function inside your code. It's essentially a function without a name, defined inline.

Lambda's have captures, which can be used to grab variables from the outer scope. It a variable is passed in the caputure, it is passed by value by default. You can also pass it by reference in the capture. They are useful for short local functions, passed to algorithms or callbacks.

- Range-based for loops and structured bindings

Range-based for loops allow iterating over containers, without explicitly using an iterator or index. `for (const auto& num: nums)`.

Structured bindings let you unpack multiple values returned from tuple-like objects (like std::pair, std::tuple, or a struct). `auto [var1, var2, var3] = tuple_or_struct`. By default, the values are copies, so you could use `auto&` to get references (for modification).

Structured bindings work well with range based operators. Imagine `std::map<std::string, int> ages`, then `for (const auto& [name, age]: ages)` works well.

- emplace_back vs push_back

Both are used to add an element to the end of an STL container, but differ in how element is created/inserted.

`push_back` takes a fully constructed object first (even if just a temporary is constructed), then moves or copies it into the container.

`emplace_back` constructs the object directly in the container; no temporary object + no copy/move (if possible). For smart pointers, they already need to get constructed outside, forming a temporary. In this case, `push_back` and `emplace_back` are the same.

- std::optional, std::variant, and std::any

`std::optional<T>`: When a value may either be absent or present. Can use `.has_value()` to check existance, and `.value()` to either get the value or throw.

`std::variant<T, Y>`: Type safe unions.. the variable can hold exactly one of any of these types. You can check which type is being held.

`std::any`: Type erased container to store any type of data, without needing to know at compile time. C++ safe version of `void*`, with automatic type management. Do not prefer this. If you know the types, use `std::variant`. 


## D. Templates and Compile-Time Mechanics

- Function and class templates

Templates let you write generic code, that works with any data type without repeating the same logic for every type. A function template defined a pattern for a function, with 1+ data types as parameters: `template <typename T>`. The compiler will automatically generate a specialized template for the type of the function when called.

Class templates let you write classes , where the attributes of the class have data types that are parameters. The compiler again will generate the right combination.

Templates enable compile-time polymorphism.. optimized code for each type used (rather than runtime polymorphism, which has some overhead).

Normally, templated things are defined right in the header file, since the compiler needs the whole definition to generate the specific type when needed. The compiler needs access when it finds the usage of a template (this makes sense). 


- Template specialization

Template specialization lets you customize how templates behave for specific types.

For function template specialization, you'd have the generic template, and then `template <>` followed by `void function_name<SpecificType>(SpecificType arg)`. This way, you would write some specialization for that type, if it doesn't follow the generic rules.

Class template specialization is similar, except you can also have partial specialization for some subset of params. For example, if 2 arg types are the same, treat it some special case. 

- SFINAE (conceptually)

Means Substituation Failure Is Not an Error. Conceptually, if the compiler tries one set of types and fails... it just ignores that template type (and will not produce an error). If the first overload you try fails, try another and see if that works (unless there are no more to try). 

- typename vs class in templates

typename and class can be essentially used interchangeably in templates, but typename is prefer to prevent ambiguity of class. Also, if you define something that uses an element in a container, you need to say `typename T::value_type first = container[0]`, where `container` would be some generic container. 

- When templates are instantiated

Recall that templates are not defining the function, just a recipie for the function. They are instantiated when the compiler sees you use them with a specific type (ie. call the template function with params, that have specific types).If the template is never used, no code is generated and it will take 0 space in the compiled binary. 

- Basics of concepts (C++20)

Essentially rules for what template types must be able to do (constraints on the template parameters). One major benefit is that they make things easier to debug, rather than having a long confusing error message for a template implementation failing.

## E. Standard Library and Containers

- Iterators (random access vs bidirectional vs input/output)

Iterators behave like pointers; allows traversal of elements in containers and to return their value. There's a few different types (although I mainly use random access iterators)...

Input iterators: Can only read, used for read only streams.

Output iterators: Can only write, used for write only streams.

Forward iterators: Can read/write, but only move forward through the container. Can be used for unordered map/set. 

Bidirection access iterator: Read/write, also move fwd + bwd. Used for map/set. 

Random access iterator. Read/write, fwd + bwd AND also random jump access. Used for vector, deque, array, string.

This matters because algorithms in STL specify what type of iterator they need (more on that later)...

- How std::vector, std::map, and std::unordered_map work conceptually

`std::vector`: A dynamically allocated array of contigous memory; resizes as needed. When `.push_back()` is called, if `size > capacity` must allocate new larger memory, copies/moves all other elements, resulting in `O(n)` operation, rather than `O(1)` normally. Favour setting a capacity beforehand if it is known. Indexing is `O(1)`, insert/remove at end is `O(1)` in the long run (amortized). Inserting/removing in middle is `O(n)` due to cost of shift. Usage: Fast, sequential operations. Order matters. Random indexing. Push/pop off the end.

`std::map`: Associative container that stores key-value pair in an ordered fashion. The underlying data structure is a red-black tree... which is a self balanced, binary search tree (BST). There is some overhead in the tree storage. You would traverse the tree with sorted keys. insert, erase, find are `O(logn)` while iteration is `O(n)`.

`std::unordered_map`: Hash-table with container storing key-value pairs; no explicit ordering. Works by holding a hash table, which is an array of buckets. A hash function will hash from key -> bucket. Elements in the same bucket are stored in linked list fashion. When items/bucket gets too high, it rehashes and stores elements differnetly to prevent hash collisions. Normally, insert, erase, and find are `O(1)`, but can be `O(n)` if they need to re-hash. Again, iteration is `O(n)`.

- When to use emplace vs insert

Emplace is used when constructing a new object for arguments and want to avoid a temporary. Rather than (Construct -> Copy/Move, you can just construct in place). Insert is used when you already have an object and want to add in into the container. Emplace saves you expensive copies/moves for more complex objects. 

- How std::sort works (introsort)

Not getting into the weeds... introsort essentially combines quicksort, heapsort, and insertion sort. It starts with quicksort, which is fast in practice `O(nlogn)`. If recursion is too deep (worst case `O(n^2)`), switches to heapsort, which is worst case `O(nlogn)`. Finally, for small partitions, it performs insertion sort. Bottom line: It's fast + memory efficient with worst case time complexity of `O(nlogn)` and space complexity `O(logn)`. 

- Complexity tradeoffs (lookup, insertion, iteration)

Note: Deque is segmented array of blocks, no need to copy entire blocks into contiguous memory (like vector). Has efficient insert/remove at front or back. Good for double ended operations.

| Container           | When to Use                                                                         |
| ------------------- | ----------------------------------------------------------------------------------- |
| **`vector`**        | When you need fast random access and append operations                              |
| **`deque`**         | When you need fast push/pop at both ends                                            |
| **`list`**          | When you need frequent insertion/removal in the middle and don’t need random access |
| **`map`**           | When you need sorted key-value pairs                                                |
| **`unordered_map`** | When you need fast lookups and order doesn’t matter                                 |
| **`set`**           | When you need sorted unique keys                                                    |
| **`unordered_set`** | When you need unique keys, order doesn’t matter                                     |
| **`array`**         | When size is fixed at compile time and contiguous memory is needed                  |

| Container       | Access   | Insert    | Erase     | Iteration | Order     |
| --------------- | -------- | --------- | --------- | --------- | --------- |
| `vector`        | O(1)     | O(n)      | O(n)      | O(n)      | Insertion |
| `list`          | O(n)     | O(1)      | O(1)      | O(n)      | Insertion |
| `deque`         | O(1)     | O(1) ends | O(1) ends | O(n)      | Insertion |
| `map`           | O(log n) | O(log n)  | O(log n)  | O(n)      | Sorted    |
| `unordered_map` | O(1) avg | O(1) avg  | O(1) avg  | O(n)      | Unordered |


## F. Performance and Low-Level Details

- Shallow vs deep copy

- What inline functions are

- When the compiler generates copy/move constructors automatically

- new vs malloc

- What happens during object construction/destruction order (base → derived → members)

- What volatile means and when to use it