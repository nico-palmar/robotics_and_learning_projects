class Solution {
public:
    vector<int> dailyTemperatures(vector<int>& temperatures) {
        // how to get the numbers of days before you get a warmer temp?
        // do it in O(n) time complexity using a monotonic stack
        // want to record indicies (in order), which will contain elements in decreasing order
        // then, we pop off the stack, and check if it's warmer to get an answer (or push onto stack)
        stack<int> decreasing_temp_idx;
        vector<int> res(temperatures.size(), 0);
        for (int i  = 0; i < temperatures.size(); i++)
        {
            // ensure that we pop off all possible values with a highest temperature
            while (!decreasing_temp_idx.empty() && temperatures[decreasing_temp_idx.top()] < temperatures[i])
            {
                // we have found the first increasing day, record it in the result vector
                res[decreasing_temp_idx.top()] = i - decreasing_temp_idx.top();
                decreasing_temp_idx.pop();
            }
            // always push on, since we don't know if it will decrease or increase in the future
            decreasing_temp_idx.push(i);
        }
        return res;
    }
};