#include <stdlib.h>
#include <stdio.h>
#include "mylist.h"

void mylist_allocate_and_add(FLOW_ITEM *p_last_item, uint32_t src_ip, uint32_t dst_ip, uint16_t dst_port){
	FLOW_ITEM *pList = (FLOW_ITEM *)malloc(sizeof(FLOW_ITEM));
	list_add(&(pList->list), &(p_last_item->list));
}

FLOW_ITEM *mylist_find_same(FLOW_ITEM *p_root_node, const FLOW_ITEM *p_item){
	FLOW_ITEM *tmp;
	FLOW_ITEM *res;
	list_for_each_entry(tmp, &p_root_node->list, list)
		printf("src= %d dst= %d\n", tmp->src_ip, tmp->dst_ip);
	return res;
}

void mylist_free(FLOW_ITEM *p_root_node){
	FLOW_ITEM *tmp;
	struct list_head *pos, *q;
	list_for_each_safe(pos, q, &(p_root_node->list)){
		tmp= list_entry(pos, FLOW_ITEM, list);
		printf("freeing item srcip= %d dstip= %d\n", tmp->src_ip, tmp->dst_ip);
		list_del(pos);
		free(tmp);
	}
}
