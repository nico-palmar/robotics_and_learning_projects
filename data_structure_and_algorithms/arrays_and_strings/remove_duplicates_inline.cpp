#include <vector>
#include <algorithm>

int removeDuplicates(std::vector<int>& nums)
{
    std::sort(nums.begin(), nums.end());
    // slow keeps track of the last unique element
    std::size_t slow = 0;

    for (std::size_t fast = 1; fast < (nums.size() - 1); fast++)
    {
        // check if we find a unique element
        if (nums[slow] != nums[fast])
        {
            // move to the next free spot and set it
            slow++;
            nums[slow] = nums[fast];
        }
    }
    // returns the last unique element in the vector; [0, slow] inclusive are unique
    return slow;
}