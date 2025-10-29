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
        slow_prev->next = nullptr;
        return slow;
    }

    ListNode* combineOrdered(ListNode* l1, ListNode* l2)
    {
        if (!l1) return l2;
        if (!l2) return l1;
        if (l1->val > l2->val)
        {
            auto next_head = combineOrdered(l1, l2->next);
            l2->next = next_head;
            return l2;
        }
        else
        {
            auto next_head = combineOrdered(l1->next, l2);
            l1->next = next_head;
            return l1;
        }
    }

    ListNode* sortList(ListNode* head)
    {
        // try to implement merge sort with the linked list
        // first we need to split the list into 2
        // note: iterative merge sort would give the desired solution
        // base case: null head
        if (!head || !head->next) return head;
        // recursive case
        auto middle = splitList(head);
        return combineOrdered(sortList(head), sortList(middle));
    }
};