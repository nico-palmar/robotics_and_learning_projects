// use the factory method design principle to create a shape factory

// first: think of steps for factory method
// 1. Create a product, in this case shape

#include <iostream>
#include <memory>

class Shape
{
public:
    virtual void describe() = 0;
};

// 2. Create concrete products... things the factory can build
class Circle: public Shape
{
public:
    void describe() override
    {
        std::cout << "We are using a circle" << std::endl;
    }
};

class Triangle: public Shape
{
public:
    void describe() override
    {
        std::cout << "We are using a triangle" << std::endl;
    }
};

// 3. Create a creator
class ShapeFactory
{
public:
    virtual std::unique_ptr<Shape> createShape() = 0;

    // can use the shape right from the factory
    // or just pull out the shape created and use it in your code
    
};

// 4. Create a concrete creator, will actually make the shape
class CircleCreator: public ShapeFactory
{
public:
    std::unique_ptr<Shape> createShape() override
    {
        auto circle = std::make_unique<Circle>();
        return circle;
    }
};

class TriangleCreator: public ShapeFactory
{
public:
    std::unique_ptr<Shape> createShape() override
    {
        auto triangle = std::make_unique<Triangle>();
        return triangle;
    }
};

int main()
{
    // now... use the factory
    auto triange_creator = std::make_unique<TriangleCreator>();
    auto triangle = triange_creator->createShape();
    triangle->describe();

    auto circle_creator = std::make_unique<CircleCreator>();
    auto circle = circle_creator->createShape();
    circle->describe();
}