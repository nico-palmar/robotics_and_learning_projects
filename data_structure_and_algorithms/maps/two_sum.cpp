class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int, int> comp_map;

        for (size_t i = 0; i < nums.size(); i++)
        {
            const auto complement = target-nums[i];
            if (comp_map.contains(complement))
            {
                vector<int> ret;
                ret.push_back(comp_map[complement]);
                ret.push_back(i);
                return ret;
            }
            else
            {
                comp_map[nums[i]] = i;
            }
        }
        throw runtime_error("No target found; not returning");
    }
};