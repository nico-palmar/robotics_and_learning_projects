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

- destructor: `~MyClass()`

Special member function, automatically invoked when an object of the class is destroyed (out of scope, or delete called on dynamically allocated memory). In charge of resource deallocation (especially dynamic memory), closing file handles, or releasing any other aquired resources.


- move constructor: `ClassName(ClassName&& other);`

Uses `&&` to create rvalue (temp value). It's used to move the source object to the target object, making it more efficient than copying. This is useful for temporary objects, or working with large data (copying large data is expensive). Usage: `std::move(object)` is used to cast lvalue to rvalue reference... so that the move constructor can be selected.

- move assignment operator: `ClassName& operator=(ClassName&& other) noexcept;`

Same ideas as difference between copy assignment operator vs constructor. Efficiently assign contents from one object to another, without making a copy. 

- override, final, and virtual keywords

Override => Used in derived class to explicitly indicate that it is overriding a virtual function from a base class (optional usage; good practice).

Final => Prevents further overriding of a virtual function in a derived class. For a class, it prevents the class from being used further in inheritance. Note: `void foo() final` is equivalent to  `virtual void foo() final` in a derived class. You just don't need to write the virtual in a derived class bc that is implicit. 

Virtual => Used to enable runtime polymorphism; signals to compiler that a derived class my override the method, and that the actual implementation should depend on the actual type of object being pointed to (made clear at runtime).

- Slicing problem

Occurs when object of derived class is assigned or copied to an obj of the base class (by value)... which results in losing derived data members + virtual function overrides. Eg. Assume Derived class is defined. Then `Derived a` `Base b = a`, and b here becomes sliced. To prevent, can use smart pointers to manage dynamically allocated objects and prevent slicing... `Derived a` `unique_ptr<Base> b = std::make_unique<Base>(a)` should work fine. 

## C. Modern C++

Move semantics (std::move, std::forward)

Rvalue references vs lvalue references

auto, decltype, constexpr, and enum class

Lambda expressions and captures

Range-based for loops and structured bindings

emplace_back vs push_back

std::optional, std::variant, and std::any

## D. Templates and Compile-Time Mechanics

Function and class templates

Template specialization

SFINAE (conceptually)

typename vs class in templates

When templates are instantiated

Basics of concepts (C++20)

## E. Standard Library and Containers

Iterators (random access vs bidirectional vs input/output)

How std::vector, std::map, and std::unordered_map work conceptually

When to use emplace vs insert

How std::sort works (introsort)

Complexity tradeoffs (lookup, insertion, iteration)

## F. Performance and Low-Level Details

Shallow vs deep copy

What inline functions are

When the compiler generates copy/move constructors automatically

new vs malloc

What happens during object construction/destruction order (base → derived → members)

What volatile means and when to use it