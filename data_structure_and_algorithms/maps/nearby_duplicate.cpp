// idea:
// keep a dict that goes from numer -> last seen index
// if we see it again, check the distance... if > k it's true.
// also reset the seen idx

class Solution {
public:
    bool containsNearbyDuplicate(vector<int>& nums, int k) {
        std::unordered_map<int, int> last_seen;

        for (std::size_t i = 0; i < nums.size(); i++)
        {
            if (last_seen.contains(nums[i]) && (i - last_seen[nums[i]]) <= k)
            {
                return true;
            }
            last_seen[nums[i]] = i;
        }
        return false;
    }
};