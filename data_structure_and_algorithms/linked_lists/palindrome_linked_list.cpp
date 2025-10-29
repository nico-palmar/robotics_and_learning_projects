/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution {
public:
    ListNode* splitList(ListNode* head)
    {
        auto slow = head;
        auto fast = head;
        ListNode* slow_prev = nullptr;
        while (fast && fast->next)
        {
            fast = fast->next->next;
            slow_prev = slow;
            slow = slow->next;
        }
        // break up the linked list
        if (slow_prev)
        {
            slow_prev->next = nullptr;
        }  
        return slow;
    }

    ListNode* reverse(ListNode* head)
    {
        if (!head || !head->next)
        {
            return head;
        }
        ListNode* new_head = reverse(head->next);
        head->next->next = head;
        head->next = nullptr;
        return new_head;
    }

    bool isPalindrome(ListNode* head) {
        // method 1: create a reversed copy
        // check that the reversed copies match. O(n) time complexity. O(n) space complexity
        // better: if we can find the middle and reverse it (second half)
        // then just compare to halfs. O(1) space complexity
        if (!head)
        {
            return false;
        }

        auto second_half_head = splitList(head);
        auto second_half_reversed = reverse(second_half_head);

        while(head && second_half_reversed)
        {
            if (head->val != second_half_reversed->val)
            {
                return false;
            }
            head = head->next;
            second_half_reversed = second_half_reversed->next;
        }
        return true;
    }
};