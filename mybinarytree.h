/* 
 * File:   mybinarytree.h
 * Author: bashmak
 *
 * Created on 29 апреля 2017 г., 13:11
 */

#ifndef MYBINARYTREE_H
#define MYBINARYTREE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>


typedef struct _TREE_ELEMENT{
    struct _TREE_ELEMENT *p_left;
    struct _TREE_ELEMENT *p_right;
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t dst_port;
    unsigned int octets;
    unsigned int packets;
} TREE_ELEMENT;


/*
 * Проверяю что ip в локальной сети 10.0.0.0/8
 */
#define is_ip_in_local_net(ip) \
    (ip >> 24 == 10)
    //((ip) > 0xa000001 && (ip) < 0xafffffe)



/*
 * Инициилизирует дерево начальными значениями
 */
#define tree_init_tree(p_tree) \
    memset(p_tree, 0, sizeof(TREE_ELEMENT))



/*
 * Пытаемся найти в дереве @p_tree лист @p_item, если не
 * находим то создаём новый лист из копии @p_item.
 * Если находим то инкрементим поля octets и packets.
 */
void tree_find_item(TREE_ELEMENT *p_tree, const TREE_ELEMENT *p_item);




/*
 * Если is_mem_dynamic=true то пытаемся удалить переданный блок с помощью free
 */
void tree_free(TREE_ELEMENT *p_tree, bool is_mem_dynamic);



#ifdef __cplusplus
}
#endif

#endif /* MYBINARYTREE_H */

