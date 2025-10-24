#include <vector>
#include <algorithm>

int find_pair_sum(const std::vector<int>& nums, const int& target)
{
    std::sort(nums.begin(), nums.end());
    
    std::size_t left = 0;
    std::size_t right = nums.size() - 1;

    while(left < right)
    {
        const auto sum = nums[left] + nums[right];
        if (sum == target)
        {
            return true;
        }
        else if (sum > target)
        {
            // sum is too large, need to decrease
            // move the right pointer to make smaller
            right--;
        }
        else
        {
            left++;
        }
    }
    return false;
}