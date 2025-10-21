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

#### D. Singleton

#### E. Pros/Cons of Different Approaches (Can leave this for later...)

## 3. Structural Patterns 1

## 4. Structural Patterns 2

## 5. Behavioural Patterns

