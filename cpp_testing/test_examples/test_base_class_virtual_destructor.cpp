#include <memory>
#include <vector>
#include <iostream>

class Interface
{
public:
    virtual void op() const = 0;

    ~Interface()
    {
        std::cout << "Base destructor" << std::endl;
    }
};

class Object: public Interface
{
public:
    Object(std::shared_ptr<int> p1, std::unique_ptr<int> p2): p1_(p1), p2_(std::move(p2)) {}

    ~Object()
    {
        std::cout << "Derived destructor" << std::endl;
    }

    void op() const override
    {
        return;
    }
private:
    std::shared_ptr<int> p1_;
    std::unique_ptr<int> p2_;
};

class Client
{
public:
    void addObj(std::shared_ptr<Interface> ptr)
    {
        vect.push_back(ptr);
    }

private:
    std::vector<std::shared_ptr<Interface>> vect;
};


int main()
{
    Client c;

    c.addObj(std::make_shared<Object>(std::make_shared<int>(5), std::make_unique<int>(6)));
    // check for destruction here?
}