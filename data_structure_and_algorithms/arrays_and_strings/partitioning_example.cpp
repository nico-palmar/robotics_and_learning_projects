#include <vector>
#include <iostream>

void moveZerosToEnd(std::vector<int>& nums)
{
    std::size_t store = 0;
    for (std::size_t curr = 0; curr < nums.size(); curr++)
    {
        if (nums[curr] != 0)
        {
            // swap in place
            // note: all elements before store are non-zero, so zero's are left after
            std::swap(nums[curr], nums[store]);
            store++;
        }
    }
}

void moveZerosToStart(std::vector<int>& nums)
{
    if (nums.empty())
    {
        return;
    }

    // now, we want all elements after store to be non-zero
    std::size_t store = nums.size() - 1;
    for (int curr = nums.size() - 1; curr >= 0; curr--)
    {
        if (nums[curr] != 0)
        {
            // swap in place
            // note: all elements before store are non-zero, so zero's are left after
            std::swap(nums[curr], nums[store]);
            if (store > 0)
            {
                store--;
            }
            
        }
    }
}

void print(const std::vector<int>& v)
{
    std::cout << "{";
    for (const auto num: v)
    {
        std::cout << num << ", ";
    }
    std::cout << "}";
}

int main()
{
    std::vector<int> v1 {1, 0, 5, 7, 8, 3, 1, 0 };
    std::vector<int> v2 {0, 3, 4, 5, 3 };
    std::vector<int> v3 {1, 4, 0, 4, 0, 2, 0, 0, 0, 3, 1};
    moveZerosToStart(v1);
    moveZerosToStart(v2);
    moveZerosToStart(v3);

    print(v1);
    print(v2);
    print(v3);
}