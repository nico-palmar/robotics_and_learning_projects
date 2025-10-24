#include <vector>
#include <algorithm>
#include <iostream>

int longest_increasing_subsequence(const std::vector<int>& sequence)
{
    // initalize all lengths to 1
    std::vector<int> lengths_up_to(sequence.size() , 1);

    for (auto i = 1; i < sequence.size(); i++)
    {
        // note: we use the ith node as the node we look up to
        // auto max_len_up_to_i = 0;
        for (auto j = 0; j < i; j++)
        {
            if(sequence[i] > sequence[j])
            {
                // max_len_up_to_i = std::max(max_len_up_to_i, lengths_up_to[j]);
                lengths_up_to[i] = std::max(lengths_up_to[i], 1 + lengths_up_to[j]);
            }
        }
        // lengths_up_to[i] = 1 + max_len_up_to_i;
    }

    // return the max element in the vector
    return *std::max_element(lengths_up_to.begin(), lengths_up_to.end());
}

int main()
{
    std::vector<int> test_1 { 3, 2, 8, 9, 7 };
    std::vector<int> test_2 { 2, 4, 3, 5, 1, 7, 10 };
    std::cout << longest_increasing_subsequence(test_1) << std::endl;
    std::cout << longest_increasing_subsequence(test_2) << std::endl;
}



