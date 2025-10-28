#include <string>
#include <unordered_map>

bool isAnagram(std::string s, std::string t)
{
    if (s.length() != t.length())
    {
        return false;
    }

    std::unordered_map<char, int> char_count;
    for (const auto& c: s)
    {
        char_count[c]++;
    }
    for(const auto& c: t)
    {
        char_count[c]--;
        if (char_count[c] < 0)
        {
            return false;
        }
    }
    return true;
}