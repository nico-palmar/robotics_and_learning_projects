#include <string>
#include <vector>
int lenLongestSubstringNoRepeats(std::string s)
{
    // keep track of char frequency using char operations
    // keep track of frequencies in ascii
    // initialize all frequencies to 0
    std::vector<int> char_freq(128, 0);
    std::size_t left = 0;
    std::size_t right = 0;
    int max_len = 1;

    for (; right < s.length(); right++)
    {
        const auto new_char = s[right];
        char_freq[new_char] += 1;
        // new character pushed us over, must get back in limits
        // we must find the last place where there was a duplicate, and slide the window back over
        while (char_freq[new_char] > 1)
        {
            char_freq[s[left]] -= 1;
            left++;
        }
        int curr_window_len = static_cast<int>(right - left) + 1;
        max_len = std::max(max_len, curr_window_len);
    }
    return max_len;
}