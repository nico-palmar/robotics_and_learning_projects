#include <iostream>
#include <memory>
#include <string>

struct Person
{
    int age;
    std::string name;
};

int main()
{
    Person nico {22, "nico"};
    std::shared_ptr<Person> test_ptr = std::make_shared<Person>(nico);

    std::cout << test_ptr->name << std::endl;

    return 0;
}