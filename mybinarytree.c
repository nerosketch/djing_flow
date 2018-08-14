#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include "mybinarytree.h"


#define GT      1       // Больше
#define EQ      0       // Равно
#define LT     -1       // Меньше


/*
 * Сравниваем элемент.
 * Если 
 */
static inline char tree_compare_element(const TREE_ELEMENT *a, const TREE_ELEMENT *b)
{
    char r;
    if(a->ip < b->ip)
        r = GT;
    else if(a->ip > b->ip)
        r = LT;
    else
        r = EQ;
    return r;
}


TREE_ELEMENT *tree_allocate_new(const TREE_ELEMENT *p_item)
{
    ssize_t s = sizeof(TREE_ELEMENT);
    TREE_ELEMENT *p_new = (TREE_ELEMENT *)malloc(s);
    memcpy(p_new, p_item, s);
    p_new->p_left = NULL;
    p_new->p_right = NULL;
    return p_new;
}


/*
 * Обходим дерево в направлени от крайних листьев к корню,
 * и передаём каждый лист в func
 */
void tree_bypass_leafs(TREE_ELEMENT *p_tree, void (*func)(const TREE_ELEMENT*, const time_t*), const time_t *p_current_timestamp)
{
    if(p_tree == NULL)
        return;

    if(p_tree->p_left != NULL)
        tree_bypass_leafs(p_tree->p_left, func, p_current_timestamp);

    if(p_tree->p_right != NULL)
        tree_bypass_leafs(p_tree->p_right, func, p_current_timestamp);

    func(p_tree, p_current_timestamp);
}


/*
 * Пытаемся найти в дереве @p_tree лист @p_item, если не
 * находим то создаём новый лист из копии @p_item.
 * Если находим то инкрементим поля octets и packets.
 */
void tree_find_item(TREE_ELEMENT *p_tree, const TREE_ELEMENT *p_item)
{
    TREE_ELEMENT *p_tmp = p_tree;
    while(p_tmp != NULL)
    {
        char r = tree_compare_element(p_tmp, p_item);

        // Если больше
        if(r == GT)
        {
            // если следующего элемента нет то это листок, не нашли
            if(p_tmp->p_right == NULL)
            {
                p_tmp->p_right = tree_allocate_new(p_item);
                break;
            }
            p_tmp = p_tmp->p_right;
        }else

        // Если меньше
        if(r == LT)
        {
            // если следующего элемента нет то это листок, не нашли
            if(p_tmp->p_left == NULL)
            {
                p_tmp->p_left = tree_allocate_new(p_item);
                break;
            }
            p_tmp = p_tmp->p_left;
        }else
        {
            // Иначе равно
            p_tmp->octets += p_item->octets;
            p_tmp->packets += p_item->packets;
            break;
        }
    }
}


/*
 * Если is_mem_dynamic=true то пытаемся удалить переданный блок с помощью free
 */
void tree_free(TREE_ELEMENT *p_tree, bool is_mem_dynamic)
{
    if(p_tree == NULL)
        return;

    if(p_tree->p_left != NULL)
        tree_free(p_tree->p_left, true);

    if(p_tree->p_right != NULL)
        tree_free(p_tree->p_right, true);

    if(is_mem_dynamic)
        free(p_tree);
    else
        memset(p_tree, 0, sizeof(TREE_ELEMENT));

}
