class Solution {
public:
    int evalRPN(vector<string>& tokens) {
        stack<int> value_stack;
        string operators = "+-*/";
        for (const auto& token: tokens)
        {
            if (operators.find(token) != string::npos)
            {
                const auto b = value_stack.top();
                value_stack.pop();
                const auto a = value_stack.top();
                value_stack.pop();
                switch (token[0])
                {
                    case '+':
                        value_stack.push(a+b);
                        break;
                    case '-':
                        value_stack.push(a-b);
                        break;
                    case '/':
                        value_stack.push(a/b);
                        break;
                    case '*':
                        value_stack.push(a*b);
                        break;
                    default:
                        // shouldn't happen
                        break;
                }
            }
            else
            {
                value_stack.push(stoi(token));
            }
        }
        return value_stack.top();
    }
};