#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));

    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);

    return head;
}

/* Delete an entry from queue
 * @entry should be a valid pointer to element_t
 */
static void q_delete_entry(element_t *entry)
{
    list_del(&entry->list);
    if (entry->value)
        free(entry->value);
    free(entry);
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    if (list_empty(head)) {
        free(head);
        return;
    }

    element_t *entry = NULL, *safe = NULL;
    /* cppcheck-suppress unknownMacro */
    list_for_each_entry_safe (entry, safe, head, list)
        q_delete_entry(entry);
    free(head);
}

/* Insert an element at head of queue */
/* cppcheck-suppress constParameterPointer */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }

    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;

    e->value = strdup(s);
    if (!e->value) {
        free(e);
        return false;
    }
    list_add(&e->list, head);
    /* cppcheck-suppress memleak */
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }

    return q_insert_head(head->prev, s);
}

/* Remove an element from head of queue */
/* cppcheck-suppress constParameterPointer */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *e = list_first_entry(head, element_t, list);
    if (sp && (e->value))
        *(char *) stpncpy(sp, e->value, bufsize - 1) = '\0';

    list_del(&e->list);
    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    return q_remove_head(head->prev->prev, sp, bufsize);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *node;
    int cnt = 0;
    list_for_each (node, head)
        cnt++;

    return cnt;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *left = head->next, *right = head->prev;
    while ((left != right) && (right->next != left)) {
        left = left->next;
        right = right->prev;
    }

    q_delete_entry(list_entry(left, element_t, list));

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node = head;
    while ((node->next != head) && (node->next->next != head)) {
        list_move(node->next->next, node);
        node = node->next->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        *(uintptr_t *) &node->prev ^= (uintptr_t) node->next;
        *(uintptr_t *) &node->next ^= (uintptr_t) node->prev;
        *(uintptr_t *) &node->prev ^= (uintptr_t) node->next;
    }
    *(uintptr_t *) &head->prev ^= (uintptr_t) head->next;
    *(uintptr_t *) &head->next ^= (uintptr_t) head->prev;
    *(uintptr_t *) &head->prev ^= (uintptr_t) head->next;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge head2 to head1
 * They must be two list without head,
 * and the next of the last node must be NULL
 */
static struct list_head *q_merge_two(struct list_head *head1,
                                     struct list_head *head2,
                                     bool descend)
{
    if (!head1 && !head2)
        return NULL;

    LIST_HEAD(head);
    while (head1 && head2) {
        char *str1, *str2;
        str1 = list_entry(head1, element_t, list)->value;
        str2 = list_entry(head2, element_t, list)->value;
        struct list_head **it =
            (((strcmp(str1, str2)) < 0) ^ descend) ? (&head1) : (&head2);
        struct list_head *safe = (*it)->next;
        list_add_tail(*it, &head);
        *it = safe;
    }

    *(uintptr_t *) &head1 ^= (uintptr_t) head2;
    while (head1) {
        struct list_head *safe = head1->next;
        list_add_tail(head1, &head);
        head1 = safe;
    }

    head.prev->next = NULL;
    return head.next;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
