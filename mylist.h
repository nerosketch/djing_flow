#ifndef _MYLIST_H_
#define _MYLIST_H_

#include <stdint.h>
#include "list.h"


typedef struct _FLOW_ITEM{
    struct list_head list;
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t dst_port;
} FLOW_ITEM;

void inline mylist_init_list(FLOW_ITEM *p_list){
    INIT_LIST_HEAD(&p_list->list);
}

void mylist_allocate_and_add(FLOW_ITEM *p_last_item, uint32_t src_ip, uint32_t dst_ip, uint16_t dst_port);

FLOW_ITEM *mylist_find_same(FLOW_ITEM *p_root_node, const FLOW_ITEM *p_item);

void mylist_free(FLOW_ITEM *p_root_node);


#endif
