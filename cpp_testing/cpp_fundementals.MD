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
    3. `unique_ptr`: Observes a shared pointer without owning; can interact with the shared pointer temporarily.
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

## B. Object Oriented Programming
