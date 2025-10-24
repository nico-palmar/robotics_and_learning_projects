## 1. Foundations and Design Principles

#### A. SOLID Design Principles

SOLID => 5 foundational OOP concepts

1. S = Single Responsability Principle: A class should only have one reason to change => it has one job. Assign only individual, concrete jobs to a class.

Helps with maintainability; uses classes/namespaces and separation of concerns.

2. O = Open-Closed Principle: Code should be open for extension but closed for modification. We should be able to extend w/ classes + interfaces + composition. But not change old code.

Helps with extensibility; uses inheritance, virtual functions, templates

3. L = Liskov Substitution Principle: A derived class should be able to replace a base class, and the code should just work.

Helps with heirarchy design; uses polymorphism + const correctness.

4. I = Interface Segregation Principle: Clients should not depend on interfaces they don't use; should not be forced to implement methods they don't need.

Helps with decoupling; uses multiple inheritance, pure virtual classes.

5. D = Dependency Inversion Principle: Depends on abstractions, not implementations. High level module should not depend on lower level module, but rather abstraction of a lower level module.

Helps with flexibility + testability; uses abstract base classes, dependency injection, and smart pointers.

How does SOLID show up in something like ROS?
- ROS: Nodes follow SRP. Topics follow DIP (no need to know underlying implementation, only interface to communicate with higher level software)
- Nav2: Plugin-based architecture (OCP). This is because we can add new plugins (OCP). Supposedly nav2_core adhers to ISP (I guess multilpe different types of clean interfaces defined).
- BTs: Each node handles one behaviour (SRP), tree can add new nodes w/o modifying the tree logic (OCP).


#### B. Identify classes, interfaces, and relationships + Interfaces and Ownership

Class => define data + behaviour together. May be instantiable. Class/struct in C++.
Interfaces => Defines what can be done, not how it can be done. Polymorphism. Class with only pure virtual methods and virtual destructor in C++.
Relationships => Define how objects interact and depend on e/o. In C++, this can be composition, inheritance, aggregation, or dependency. 

| Type               | C++ keyword or construct          | Example                                    | Relationship meaning |
| ------------------ | --------------------------------- | ------------------------------------------ | -------------------- |
| **Interface**      | Abstract class with pure virtuals | `class ISensor`                            | Defines behavior (what can be done, not how)    |
| **Concrete class** | Derived from interface            | `class TemperatureSensor : public ISensor` | Implements behavior (how it can be done)  |
| **Composition**    | Member `std::unique_ptr<ISensor>`. Members. Unique pointers. | `SmartHomeController` owns sensor          | Strong “has-a”. Composed object dies with parent object (in terms of lifetime; how a car would have strong ownership of it's engine)      |
| **Aggregation**    | `ISensor& sensor_` (no ownership). Reference. Weak pointer. Shared point? | Shares sensor object                       | Weak “has-a”. The lifetimes are independent. Similar to how a teacher has a student (student continues living on, past the teacher).         |
| **Dependency**     | Function argument                 | `void setSensor(ISensor&)`                 | “Uses”. Simply depends on for some function, but not owned in any way. Lifetime is very temporary.              |
| **Inheritance**    | `: public`                        | `TemperatureSensor : public ISensor`       | “Is-a”               |


To recap...

Interfaces => define a contract. Virtual destructors should be used for deletion via base class pointers.

Why do interfaces matter? Separate what is done from how it's done. Enables Dependency Inversion Principle (DIP) since the underlying implementation can be abstracted in higher level modules. Also allows for polymorphism.

Ownership => who is responsbile for an object's lifetime. Common cases?
1. `unique_ptr<Interface>`: Used when a class takes ownership. Car owns it's engine.
2. `shared_ptr<Interface>`: Used when ownership is shared between multiple entities. Lifetime is extended until all references are gone
3. `Interface&`: No ownership taken. Just use the object, someone else managed lifetime. Object outlives the class.

## 2. Creational Patterns
For each, study the purpose behind the pattern, and know how UML diagrams would work

#### A. Factory Method
Purpose: Define an interface for creating an object, but subclasses decide which class to instantiate. So, instantiation of objects it left to a subclass, rather than being done directly in code. Decouples the object creation from it's usage => OCP (Open/Closed Principle). 

When to use? Hide creation logic from client (base class; creator). Decide object type at runtime. 

When not to use it? Object creation is simple and not likely to change. When there is no need to encapsulate the object being created.

How does it work?
1. Create an abstract product for objects
2. Create variants of concrete products... created by factory
3. Create an abstract Creator; declares the factory method
4. Create concrete Creators; define the factory method creation using concrete products

Some common examples of usages include building loggers, or spawning elements in a game. For the game example, it decouples object creation from game logic, and enables scalability.

An alternative would be to use a bunch of if statements to define creation logic in the client class. Instead, using the factory method, a client class does not need to change it's code, it just uses the factory interface. This can easily be extended for new factory classes without altering any client code... this is OCP. 

#### B. Abstract Factory

An interface for creating families of related/dependant objects without specifying their concrete types.

Factory Method is good for one product type, Abstract Factory creates multiple related product types, meant to work together. It's essentially factory method with an even higher level abstract factory that works with a bunch of abstract products.

When to use it? When you have families of related products, to be used together (eg. GUI widgets like buttons, but for different OS). Might want code to be independant of what type of family is used.

How does it work?
1. Abstract products (no families)
2. Variants of concrete products (in families, potentially)
3. Create an abstract factory... this will contain multiple factory methods for different objects
4. Create concrete factories; define the factory creation methods.

Then, the client code will create concrete factories, which will create any needed products to be used. Client code is abstracted from specifics. This is good for OCP. 

#### C. Builder

Builder pattern is used to construct complex objects in a step-by-step fashion. This can be when the object requires multiple config steps... or we want to reuse the construction logic.

How does it work?
1. Product: The complex thing that is being built. Has a bunch of attributes, don't want a crazy constructor.
2. Builder: An interface which declares steps required to create the type of product.
3. Concrete Builders: Define implementations for the steps to actually build an object
4. Director (optional): Controls the building process via sequence of steps of builder calls.

Don't use when the code overhead is too much.. for construction of simple objects.

One key point is that either the builder or concrete builder will have a product. It must be in charge of passing that object out by having the director return the object, or exposing a method itself. To avoid expensive copies of large objects, pointers can be used rather than objects (although objects work fine in the general case).

#### D. Singleton

A singleton is a pattern that ensures only one instance of a class is created, and provides a global access point. Essentially, it is controlled global state. 

How does it work?
1. Constructor is private/protected
2. You access the singleton through a static method (which returns a static reference to the object).
3. The instance is created only once; copy constructor and copy assignment operator are deleted to ensure this.

When not to use them? Well... many scenarios. Global variables, objects with multiple instances, and concurrent systems are scenarios you wouldn't prefer singletons. It can break the DIP (dependency inversion principle). For a database manager, an interface which can provide different databases would be preferred (dependency injection). Also better from a testing perspective because you can have multiple types of the object (simulated and real), not just a hard coded singleton which is now tightly coupled wherever you use it.

When to use? When there is only exactly 1 instance, and that unique instance is inherit to the problem (not just code convenience). For example, a hardware device interface with a single connection to a port (good singleton usage).   


## 3. Structural Patterns 1

#### A. Adapter

The adapter allows classes with incompatible interfaces to work, by translating one interface "(unknown)" to another "(known)" by the client. Sometimes called wrapper; wraps one class to make compatible with another (think of a wall plug adapter).

How does it work?
1. Client want to use some Target interface (person wants to use an EU charger)
2. Adaptee has useful functionality, but uses a different interface (charger is a north american charger).
3. Adapter then bridges the gap, implements the Target interface and translates the calls into the adaptee (creates an adapter which plugs into the EU charger, while translating the other end into something the north american charger can plug into). 

Variants: Can be done via object composition (normally preferred); called the object adapter. Can also be done via inheritance, the class adapter. It inherits from the target and adaptee, so it's less flexible. 

When to use it? Legacy/third party code that doesn't match required interface. Reuse functionality without modifying code.

When not to use it? There are already too many levels of abstraction and wrapper would just provide more. Or, if you can just directly modify the adaptee (this would be equivalent to moving to europe and just changing over to a european charger, no adapter).

#### B. Bridge

The bridge pattern decouples abstraction from implementation so that the two can vary independently. Separates what you do with how you do it; uses composition and inheritance.

How does it work?
1. An absraction defines a high-level interface (a thing that defines what you do). It often contains a reference/shared pointer to an implementor.
2. An implementor defines a lower-level interface (how are you actually going to do that thing? Defines the implementation of the abstract's interface)
3. A refined abstraction will provide specific methods for the abstraction; it's what the client code will actually interact with.
4. A concrete implementor implements lower level interface. Will have specific ways to achieve underlying functionality. 

In the real world, we can think of this as a TV and remote. The remote, abstraction, will define a high level thing that can be done. Then, the TV, implementor, will define how that action is actually done. The client will actually use the refined abstraction. These can be altered individually. 

When to use it? Have 2 dimensions of variation. Or, the implementations may change frequently (hw drivers).

Avoid when: only have a single axis of variation (abstraction is fine), or if the implementor is not expected to vary. 

How it differs from Adapter?
Decouple abstraction from implementation; both are designed to work together. Generally a proactive design... in comparison, the adapter is a reactive design. Have old code and you want to make it work.


## 4. Structural Patterns 2

#### A. Composite

Composite pattern lets you deal with individual objects and composites uniformly. Essentially this is when you have a tree structure, and leaves/groups share a common interface for interaction.

How does it work?
1. Define a common interface to be used by all components
2. Define 'leaf' nodes which implement the interface
3. Define composite nodes which contain multiple components; and can implement their methods as an aggregation
4. The client code can now treat individual and groups uniformally.

When to use it? You have a heirarchical structure (trees of parts and subparts). Client should treat individual and composite parts as the same. 

When not to use it? No tree-like structure; need different interfaces for leaves/composites. Have strict ownership rules.

#### B. Decorator

Decorator allows adding new behaviour/responsabilities without modifying the existing code. It's like wrapping functionality on top of an existing component; adding layers. It implements the same interface as the thing it's wrapping.

How does it work?
1. Have a common interface for a component
2. Implement a version of the component (the thing to be wrapped)
3. Implement a decorator (the wrapper) that uses composition with a thing of the common interface. The decorator can extend the functionality of the thing it wraps.

When to use? Add more modify behaviour dynamically at runtime. Avoid subclass explosion, OCP, mix+match features freely.

When not to use? Simple subclassing is fine, the number of decorators becomes too high (hard to debug), need tight coupling. 

#### C. Facade

Facade pattern should provide a simple, unified interface to a complex subsystem. It's like a front desk for your system, that delegates the work.

How does it work?
1. You have multiple subsystems, each with their own interfaces and functionalities.
2. You want the client to interfact with the entire system together. The facade provides a single API to coordinate amongst multiple subsystems.

When to use it? Have a complex subsystem with multiple interdependent classes. Simplify client interface and reduce coupling.

When to avoid? Already have clean/simple API, would just duplicate exisiting interface, need fine control (facade would hide too many details).

## 5. Behavioural Patterns

#### A. Strategy

Encapsulates a family of algorithms (aka. strategies), allowing runtime switching without changing the code that uses them.

How does it work?
1. Define a common strategy interface.
2. Have multiple concrete implementations of the strategy.
3. Allow the context class to hold ownership to an interface and call it's method, while enabling to swap out different types of methods. Note that the context doesn't inherit the strategy interface (if it did, it would be more of a composite/decorator hybrid).

When to use it? When you have multiple algorithms for the same type and want to switch dynamically; avoid large if/switch statements. The behaviour changes frequently at run time. 

When not to uses it? Algorithm never changes, or you need lots of data sharing between algorithms (since the algorithms are isolated).

#### B. Command

The command pattern encapsulates the request/action as an object. Instead of calling the request directly, you wrap it in a command object which knows how to execute the request/action.

Allows for queueing, logging, or undoing actions easily. Also decouples the invoker (eg. button on UI) from the reciever (object performing something).

How does it work?
1. Command interface, which declares some execute method
2. Concrete commands, which implements execute by calling methods on the reciever. The concrete commands then have a receiver. 
3. Reciever, which is the object actually doing the work.
4. Invoker, which triggers the command without knowing what it does internally (like the UI, or some remote contorl). Contains an array of commands for the history. 
5. Client, which configures some (concrete) commands and assigns them to invokers. 


When to use? Need to undo/repo actions, or history tracking. Queue, schedule, or log actions, decouple senders from receivers. 

When not to use? Operations are simple, no need for abstractions. No need to undo/redo history.

Note that there is an undo/redo extension to this pattern. Essentially, you add an additional undo method for the command (so undo for takeoff is to land). Then, the invoker takes 2 extra stacks; one for executed and one for undone. If we want do undo, then pop off the execute stack, execute undo, then push the command to the undo stack. If we want to redo last, then pop off the undo stack, execute normal, and put on the execute stack. The implementation I saw used pointers for the stacks. I would just make them all shared pointers in the stacks to avoid any issues.

#### C. Observer

#### D. State

ONCE THIS IS DONE:  Pick 2-3 LLD problems. Start simple. Try them out.

