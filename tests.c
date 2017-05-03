#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#include "mybinarytree.h"
#define uint unsigned int


uint32_t ip2int(const char* ip)
{
	uint32_t res = 0;
	inet_pton(AF_INET, ip, &res);
	return htonl(res);
}


void tree_fill_item(TREE_ELEMENT *p_item, uint32_t src_ip, uint32_t dst_ip, uint16_t dst_port,
	unsigned int octets, unsigned int packets)
{
	p_item->dst_ip = dst_ip;
	p_item->src_ip = src_ip;
	p_item->dst_port = dst_port;
	p_item->octets = octets;
	p_item->packets = packets;
}



void test_print_recursive(const TREE_ELEMENT *p_tree)
{
	if(p_tree == NULL)
		return;

	if(p_tree->p_left != NULL)
		test_print_recursive(p_tree->p_left);

	if(p_tree->p_right != NULL)
		test_print_recursive(p_tree->p_right);

	printf("src_ip %u, dst_ip %u, flows %u, packets %u;\n", p_tree->src_ip, p_tree->dst_ip, p_tree->octets,
		p_tree->packets);

}


void main_test()
{
	TREE_ELEMENT items[12];
	memset(&items, 0, sizeof(TREE_ELEMENT)*12);

	tree_fill_item(&items[0], ip2int("10.12.1.10"), ip2int("192.168.0.1"), 80, 512, 5);
	tree_fill_item(&items[1], ip2int("10.12.1.10"), ip2int("192.168.0.1"), 80, 512, 5);
	tree_fill_item(&items[2], ip2int("10.12.1.7"), ip2int("192.168.0.100"), 80, 512, 5);
	tree_fill_item(&items[3], ip2int("10.12.1.10"), ip2int("192.168.0.1"), 80, 512, 5);
	tree_fill_item(&items[4], ip2int("10.12.1.11"), ip2int("192.168.0.1"), 80, 512, 5);
	tree_fill_item(&items[5], ip2int("10.12.1.11"), ip2int("192.168.0.1"), 80, 512, 5);
	tree_fill_item(&items[6], ip2int("10.12.1.7"), ip2int("192.168.0.100"), 80, 512, 5);
	tree_fill_item(&items[7], ip2int("10.12.1.11"), ip2int("192.168.0.1"), 80, 512, 5);
	tree_fill_item(&items[8], ip2int("10.12.1.11"), ip2int("192.168.0.1"), 80, 512, 5);
	tree_fill_item(&items[9], ip2int("10.12.1.7"), ip2int("192.168.0.100"), 80, 512, 5);
	tree_fill_item(&items[10], ip2int("10.12.1.11"), ip2int("192.168.0.1"), 80, 512, 5);
	tree_fill_item(&items[11], ip2int("10.12.1.11"), ip2int("192.168.0.1"), 80, 512, 5);

	TREE_ELEMENT tree;
	memcpy(&tree, &items[0], sizeof(TREE_ELEMENT));
	//tree_init_tree(&tree);

	for(uint n=1; n<12; n++)
	{
		if(is_ip_in_local_net(items[n].src_ip))
			tree_find_item(&tree, &items[n]);
	}

	test_print_recursive(&tree);

	tree_free(&tree, false);
}
