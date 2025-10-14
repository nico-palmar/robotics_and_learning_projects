#include <memory>
#include <iostream>

int main()
{
    std::unique_ptr<int> u_ptr = std::make_unique<int>(8);
    std::unique_ptr<int> u_ptr_2;

    u_ptr_2 = std::move(u_ptr);

    std::cout << "New unique ptr " << *u_ptr_2 << std::endl; 

    std::shared_ptr<int> s_ptr = std::make_shared<int>(10);
    std::shared_ptr<int> s_ptr_2 = s_ptr;
    std::weak_ptr<int> w_ptr = s_ptr;

    std::cout << "Count of shared ptr is " << s_ptr.use_count() << std::endl;

    if (w_ptr.expired())
    {
        std::cout << "All hell broke loose" << std::endl;
    }
    
    if (const auto ptr = w_ptr.lock())
    {
        std::cout << "Have weak ptr value " << *ptr << std::endl;
        // temp shared pointer is made
        std::cout << "Count of shared ptr is " << s_ptr.use_count() << std::endl;
    }
    // value of shared pointer count goes down here
    std::cout << "Count of shared ptr is " << s_ptr.use_count() << std::endl;
}