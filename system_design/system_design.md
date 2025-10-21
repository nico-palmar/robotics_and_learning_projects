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


#### B. Identify classes, interfaces, and relationships


#### C. Interfaces and Ownership in C++

## 2. Creational Patterns

## 3. Structural Patterns 1

## 4. Structural Patterns 2

## 5. Behavioural Patterns

