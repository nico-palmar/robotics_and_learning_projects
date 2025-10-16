#include <iostream>

class Engine
{
public:
    float horsepower;
    int n_cylinders;

    void runEngine()
    {
        std::cout << "VROOOM" << std::endl;
    }
};

class Vehicle
{
public:
    Engine engine;
    int n_doors;

    void turnOn()
    {
        std::cout << "Clicked on button" << std::endl;
        engine.runEngine();
    }

    virtual void warmUp() = 0;
};

class Car: public Vehicle
{
public:
    bool seat_heater_on { false };

    void warmUp() override final
    {
        seat_heater_on = true;
        std::cout << "The seat heater was turned on" << std::endl;
    }
};

int main()
{
    Car car;
    car.turnOn();
    car.warmUp();

    // TODO: investigate runtime polymorphism behaviour
    // clean up things to be better encapsulated (probably add constructors)
    // see when casting can help with polymorphism (or not)
}