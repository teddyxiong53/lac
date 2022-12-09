#ifndef _DL_LIST_H_
#define _DL_LIST_H_
#include <stdbool.h>

/**
 * @brief 双向链表。dl表示double link。
 *
 */
struct dl_list_node {
    struct dl_list_node *prev;
    struct dl_list_node *next;
};

static inline void dl_list_init(struct dl_list_node *l)
{
    l->next = l->prev = l;
}

/**
 * @brief 把节点n插入到链表节点l的后面。
 *
 * @param l
 * @param n
 */
static inline void dl_list_insert_after(struct dl_list_node *l, struct dl_list_node *n)
{
    // 先处理当前l后面的一个节点跟n的关系。也就是l->next 跟n的关系。
    // 也就l处理时，是先人后己。比较无私。
    l->next->prev = n;
    n->next = l->next;

    //再处理l跟n的关系。
    l->next = n;
    n->prev = l;
}

/**
 * @brief 把n节点插入到l节点的前面。
 *
 * @param l
 * @param n
 */
static inline void dl_list_insert_before(struct dl_list_node *l, struct dl_list_node *n)
{
    //还是先人后己的顺序处理。
    l->prev->next = n;
    n->prev = l->prev;

    l->prev = n;
    n->next = l;
}

static inline void dl_list_remove(struct dl_list_node *n)
{
    n->prev->next = n->next;
    n->next->prev = n->prev;
    n->next = n->prev = n;//这一步不要漏掉了。
}

static inline bool dl_list_isempty(struct dl_list_node *l)
{
    return l->next == l->prev;
}

static inline unsigned int dl_list_len(struct dl_list_node *l)
{
    unsigned int len = 0;
    struct dl_list_node *p = l;
    while (p->next != l) {
        p = p->next;
        len ++;
    }
    return len;
}

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

/**
 * @brief
 *
 */

#define dl_list_entry(node, type, member) \
    container_of(node, type, member)

/**
 * @brief 这个的作用不大。一般不用。
 *
 */
#define dl_list_for_each(pos, head) \
    for (pos=(head)->next; pos != (head); pos=pos->next)

/**
 * @brief 下面这个比较实用。
 *
 */
#define dl_list_for_each_entry(pos, head, member) \
    for (pos = dl_list_entry((head)->next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = dl_list_entry(pos->member.next, typeof(*pos), member))
#endif
