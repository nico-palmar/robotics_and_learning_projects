#include <iostream>

class A
{
    public:
    int x;
};

int main()
{

    A obj{ 5 };
    A obj_2 = obj;
    obj.x = 6;

    std::cout << obj.x << std::endl;
    std::cout << obj_2.x << std::endl;
}