#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <variant>
#include <map>

int main()
{
    // TODO: 
    // play with some move semantics
    // write some lambdas
    // use range based loops
    // create directly into vector with emplace back
    // make a union type with std::variant

    auto ptr = std::make_unique<int>(5);

    std::vector<std::unique_ptr<int>> v;

    // must move the ptr
    v.push_back(std::move(ptr));

    // this line segfaults
    // std::cout << *ptr << std::endl;

    std::cout << *v[0] << std::endl;

    std::variant<std::string, int> special_var {"Banana"};

    const auto crack_code = [special_var](int& data, const std::map<std::string, int>& code_mapping) {
        if (std::holds_alternative<int>(special_var))
        {
            data += std::get<int>(special_var);
        }
        else
        {
            const auto val = code_mapping.at(std::get<std::string>(special_var));
            data = val;
        }
    };

    std::map<std::string, int> mapping = {
        {"Hello", 5},
        {"Banana", 6}
    };

    auto data = 0;

    crack_code(data, mapping);
    std::cout << data << std::endl;
}