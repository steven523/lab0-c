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
    struct list_head *new = malloc(sizeof(struct list_head));
    if (new) {
        INIT_LIST_HEAD(new);
        return new;
    } else {
        return NULL;
    }
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (head) {
        element_t *entry, *safe;
        list_for_each_entry_safe (entry, safe, head, list)
            q_release_element(entry);
        free(head);
    } else
        return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    if (new) {
        new->value = strdup(s);
        if (!new->value) {
            q_release_element(new);
            return false;
        }

        list_add(&new->list, head);
        return true;
    } else {
        return false;
    }
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    if (new) {
        new->value = strdup(s);
        if (!new->value) {
            q_release_element(new);
            return false;
        }

        list_add_tail(&new->list, head);
        return true;
    } else {
        return false;
    }
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *delement = list_first_entry(head, element_t, list);
    list_del_init(head->next);

    /*If sp is non-NULL and an element is removed, copy the removed string to
     *sp (up to a maximum of bufsize-1 characters, plus a null terminator.)*/
    if (sp) {
        strncpy(sp, delement->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return delement;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *delement = list_last_entry(head, element_t, list);
    list_del_init(head->prev);

    if (sp) {
        strncpy(sp, delement->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return delement;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int len = 0;
    struct list_head *node;

    list_for_each (node, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head **indir_slow = &head->next;

    for (struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next)
        indir_slow = &(*indir_slow)->next;

    struct list_head *delement = *indir_slow;

    list_del_init(delement);
    q_release_element(list_entry(delement, element_t, list));

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    element_t *element, *next;
    bool flag = false;
    list_for_each_entry_safe (element, next, head, list) {
        if (&next->list != head && strcmp(element->value, next->value) == 0) {
            list_del_init(&element->list);
            q_release_element(element);
            flag = true;
        } else if (flag) {
            list_del_init(&element->list);
            q_release_element(element);
            flag = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *cur = head;
    struct list_head *first = head->next, *second = first->next;

    for (; first != head && second != head;) {
        first->next = second->next;
        second->prev = first->prev;
        second->next = first;
        first->prev = second;
        cur->next = second;
        first->next->prev = first;

        cur = cur->next->next;
        first = cur->next;
        second = first->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *cur, *safe;
    list_for_each_safe (cur, safe, head) {
        list_move(cur, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || k < 2)
        return;

    int len = q_size(head);
    for (struct list_head *cur = head->next; cur != head && cur->next != head;
         cur = cur->next) {
        struct list_head **reverse_node = &cur->next, *tmp = cur->prev;
        for (int i = 1; i < k; i++) {
            if (len >= k)
                list_move(*reverse_node, tmp);
        }
        len -= k;
    }
}

/* Sort elements of queue in ascending/descending order */
struct list_head *merge_list(struct list_head *left, struct list_head *right)
{
    struct list_head *head = NULL, **ptr = &head;

    for (; left && right; ptr = &(*ptr)->next) {
        if (strcmp(list_entry(left, element_t, list)->value,
                   list_entry(right, element_t, list)->value) < 0) {
            *ptr = left;
            left = left->next;
        } else {
            *ptr = right;
            right = right->next;
        }
    }

    if (left) {
        *ptr = left;
    } else {
        *ptr = right;
    }
    return head;
}

struct list_head *merge_sort(struct list_head *head)
{
    if (!head || !head->next)
        return head;
    struct list_head *fast, *slow = head;

    for (fast = head->next; fast && fast->next; fast = fast->next->next) {
        slow = slow->next;
    }

    struct list_head *left, *right;

    right = slow->next;
    slow->next = NULL;

    left = merge_sort(head);
    right = merge_sort(right);

    return merge_list(left, right);
}


void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    head->prev->next = NULL;
    head->next = merge_sort(head->next);

    struct list_head *cur = head, *n = head->next;

    while (n) {
        n->prev = cur;
        cur = n;
        n = n->next;
    }

    cur->next = head;
    head->prev = cur;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    element_t *cur, *safe;
    q_reverse(head);
    char *min = list_entry(head->next, element_t, list)->value;

    list_for_each_entry_safe (cur, safe, head, list) {
        if (strcmp(cur->value, min) > 0) {
            list_del_init(&cur->list);
            q_release_element(cur);
        } else {
            min = cur->value;
        }
    }
    q_reverse(head);
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    element_t *cur, *safe;
    q_reverse(head);
    char *max = list_entry(head->next, element_t, list)->value;

    list_for_each_entry_safe (cur, safe, head, list) {
        if (strcmp(cur->value, max) < 0) {
            list_del_init(&cur->list);
            q_release_element(cur);
        } else {
            max = cur->value;
        }
    }
    q_reverse(head);
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (list_empty(head))
        return 0;
    if (list_is_singular(head))
        return list_entry(head->next, queue_contex_t, chain)->size;

    queue_contex_t *first = list_entry(head->next, queue_contex_t, chain);
    queue_contex_t *pos = NULL;

    struct list_head *tmp = first->q->next;
    first->q->prev->next = NULL;
    list_for_each_entry (pos, head, chain) {
        if (first == pos)
            continue;
        pos->q->prev->next = NULL;
        tmp = merge_list(tmp, pos->q->next);
        INIT_LIST_HEAD(pos->q);
    }
    first->q->next = tmp;
    struct list_head *cur = first->q, *n = cur->next;

    while (n) {
        n->prev = cur;
        cur = n;
        n = n->next;
    }

    cur->next = first->q;
    first->q->prev = cur;

    return q_size(first->q);
}

// Implement q_shuffle and swap
void swap(element_t *n1, element_t *n2)
{
    char *tmp = n1->value;
    n1->value = n2->value;
    n2->value = tmp;
}
void q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *end = head->prev;

    for (int i = q_size(head); i > 0; i--) {
        int ran = rand() % i;
        struct list_head *cur = head;
        for (int j = 0; j <= ran; j++) {
            cur = cur->next;
        }
        swap(list_entry(cur, element_t, list),
             list_entry(end, element_t, list));
        end = end->prev;
    }
}
