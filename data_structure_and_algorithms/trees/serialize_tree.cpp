/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Codec {
public:

    // idea: serialize in a known ordering?
    // could use inorder traversal to construct the serialized version? -> no you can't... find you can't easily keep track of left vs right. Use preorder instead.
    // or even use level order since that's easy without recursion (no need to recurse stings)
    // then get it back out level order

    // Encodes a tree to a single string.
    string serialize(TreeNode* root) {
        if (!root)
        {
            return "#";
        }
        return to_string(root->val) + "|" + serialize(root->left) + "|" + serialize(root->right);
    }

    // Decodes your encoded data to tree.
    TreeNode* deserializeHelper(istringstream& ss)
    {
        string val;
        if (!getline(ss, val, '|')) return nullptr;
        if (val == "#") return nullptr;

        auto root = new TreeNode(stoi(val));
        root->left = deserializeHelper(ss);
        root->right = deserializeHelper(ss);
        return root;
    }


    TreeNode* deserialize(string data) {
        istringstream ss(data);
        return deserializeHelper(ss);
    }
};

// Your Codec object will be instantiated and called as such:
// Codec* ser = new Codec();
// Codec* deser = new Codec();
// string tree = ser->serialize(root);
// TreeNode* ans = deser->deserialize(tree);
// return ans;