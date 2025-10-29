class Solution {
public:
    // idea:
    // maintain a stack of open brackets
    // if you see a closing bracket, pop off the stack and check for similarity
    // if the stack isn't empty at the end, something is off
    bool isValid(string s) {
        unordered_map<char, char> bracket_pairs = {
            {'}', '{'}, {')', '('} ,{']', '['}
        };
        stack<char> char_stack;

        for (const auto parenthesis: s)
        {
            if (!bracket_pairs.contains(parenthesis))
            {
                // this is an opening bracket
                char_stack.push(parenthesis);
                continue;
            }
            const auto expected = bracket_pairs[parenthesis];
            if (char_stack.empty() || char_stack.top() != expected)
            {
                return false;
            }
            char_stack.pop();
        }
        return char_stack.empty();

    }
};