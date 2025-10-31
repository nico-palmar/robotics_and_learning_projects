/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    TreeNode* sortedArrayToBST(vector<int>& nums) {
        // base case: empty nums, return
        if (nums.empty())
        {
            return nullptr;
        }
        // idea: pick middle of array as root
        auto mid_idx = nums.size() / 2;
        auto root = new TreeNode(nums[mid_idx]);

        // build left tree
        vector<int> nums_left(nums.begin(), nums.begin() + mid_idx);
        root->left = sortedArrayToBST(nums_left);
        // build right tree
        vector<int> nums_right(nums.begin() + mid_idx + 1, nums.end());
        root->right = sortedArrayToBST(nums_right);

        return root;

        // OR note another solution... rather than mass creating vectors...
        // keep the same single vector, and add another internal function which takes left/right indicies (referring to subtree)
        // then, if left passes right, return nullptr
        // mid is left + (right-left)/2 = r/2 + l/2 = (r+l)/2.. but prefer the first
        // use the first version to prevent overflow
        // then make trees based on that
    }
};