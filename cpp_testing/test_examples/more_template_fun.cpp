#include <stdexcept>
#include <iostream>
#include <vector>

template <typename T>
T clamp(const T& value, const T& min_val, const T& max_val)
{
    if (min_val >= max_val)
    {
        throw std::runtime_error("Minimum value cannot be greater or equal to maximum value");
    }
    // val = 3, min = 5/2 3,5 => 5. Can't go below min value
    // if negative... -3, -5 => -3. -5 is a smaller value
    const auto min_corrected = std::max(value, min_val);
    const auto min_max_corrected = std::min(min_corrected, max_val);
    return min_max_corrected;
}

// add in an overload for std::vector
template <typename T>
std::vector<T> clamp(const std::vector<T>& values, const std::vector<T>& min_values, const std::vector<T>& max_values)
{
    // element wise clamp at every position
    if (values.empty())
    {
        return {};
    }

    if (values.size() != min_values.size() || values.size() != max_values.size())
    {
        throw std::runtime_error("Sizes do not align on vector-wise clamp");
    }

    std::vector<T> ret;
    ret.reserve(values.size());
    for (std::size_t i = 0; i < values.size(); i++)
    {
        ret.push_back(clamp(values[i], min_values[i], max_values[i]));
    }
    return ret;
}

int main()
{
    try
    {
        std::cout << clamp(5, 1, 10) << std::endl;
        std::cout << clamp (-3, 0, 100) << std::endl;
        std::cout << clamp(10.5, 0.0, 10.0) << std::endl;
        std::cout << clamp(20, 21, 19) << std::endl;
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << "Generic clamp failed: " << e.what() << '\n';
    }

    std::vector<int> vals { -2, -3, -50, 5, 6, 8, 100, 1000 };
    std::vector<int> min_values(vals.size(), 0);
    std::vector<int> max_values(vals.size(), 30);

    auto res = clamp(vals, min_values, max_values);

    for (const auto clamped_val: res)
    {
        std::cout << clamped_val << " ";
    }
    std::cout << std::endl;
    

    return 0;
}