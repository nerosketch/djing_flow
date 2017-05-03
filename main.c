#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <ftlib.h>
#include "conf.h"
#include "mybinarytree.h"
//#include "tests.h"


#define FLOW_COLS		8
#define uint			unsigned int


void curtime(char* pInStrTime, const uint maxlen, const char *format)
{
	time_t rawtime;
	time( &rawtime );
	strftime(pInStrTime, maxlen, format, localtime( &rawtime ));
}


/*
 * В самом начале в @first всегда передаём true
 */
void print_recursive(const TREE_ELEMENT *p_tree, time_t current_timestamp, bool first)
{
	if(p_tree == NULL)
		return;

	if(p_tree->p_left != NULL)
		print_recursive(p_tree->p_left, current_timestamp, false);

	if(p_tree->p_right != NULL)
		print_recursive(p_tree->p_right, current_timestamp, false);

	printf("(%ju,%u,%u,%u)", current_timestamp,
		p_tree->ip,  p_tree->octets, p_tree->packets);

	if(first)
		printf(";\n");
	else
		printf(",\n");

}


static inline bool fill_item_data(TREE_ELEMENT *p_item, const char *rec, struct fts3rec_offsets *fo)
{

	uint32_t dst_ip = *((uint32_t*)(rec+fo->dstaddr));
	uint32_t src_ip = *((uint32_t*)(rec+fo->srcaddr));

	p_item->octets = *((uint32_t*)(rec+fo->dOctets));
	p_item->packets = *((uint32_t*)(rec+fo->dPkts));

	if( is_ip_in_local_net( src_ip ) )
		p_item->ip = src_ip;
	else  if( is_ip_in_local_net( dst_ip ) )
		p_item->ip = dst_ip;
	else
		return false;

	return true;
}


int main()
{
	/*main_test();
	return 0;*/

	char table_name[19] = {0};
	struct ftio ftio;
	struct ftver ftv;
	char *rec;
	struct fts3rec_offsets fo;
	//struct fts3rec_all cur;
	DJING_CONF_STRUCT conf;
	
	time_t current_timestamp = time(NULL);

	memset(&conf, 0, sizeof(DJING_CONF_STRUCT));
	const char* config_fname = "./djing_flow.conf";
	if(read_config(&conf, config_fname) != E_SUCCESS){
		fprintf(stderr, "ERROR: config '%s' not read\n", config_fname);
		return -1;
	}
	
	if (ftio_init(&ftio, 0, FT_IO_FLAG_READ) < 0)
	    fterr_errx(1, "ftio_init(): failed");

	ftio_get_ver(&ftio, &ftv);
	fts3rec_compute_offsets(&fo, &ftv);
	if (ftio_check_xfield(&ftio, /*FT_XFIELD_UNIX_SECS| FT_XFIELD_UNIX_NSECS|*/
		FT_XFIELD_DPKTS | FT_XFIELD_DOCTETS | FT_XFIELD_SRCADDR | FT_XFIELD_DSTADDR |
		FT_XFIELD_SRCPORT | FT_XFIELD_DSTPORT | FT_XFIELD_PROT)) {
			fterr_warnx("Flow record missing required field for format.");
			return -1;   
	}

	curtime(table_name, 19, "flowstat_%d%m%Y");

	printf("CREATE TABLE IF NOT EXISTS %s (\n", table_name);
	printf("`cur_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\n"
		"`ip` INT(10) UNSIGNED NOT NULL,\n"\
		"`octets` INT unsigned NOT NULL DEFAULT 0,\n"\
		"`packets` INT unsigned NOT NULL DEFAULT 0\n"\
		") ENGINE=MyISAM DEFAULT CHARSET=utf8;\n");


	printf("INSERT INTO %s(`cur_time`,`ip`,`octets`,`packets`) VALUES\n", table_name);

	rec = ftio_read(&ftio);
	if(!rec){
		perror("ERROR: flow not read\n");
		ftio_close(&ftio);
		return -1;
	}


	TREE_ELEMENT tmp_item;
	tree_init_tree(&tmp_item);
	fill_item_data(&tmp_item, rec, &fo);


	TREE_ELEMENT tree_root;
	tree_init_tree(&tree_root);
	memcpy(&tree_root, &tmp_item, sizeof(TREE_ELEMENT));


	while(true)
	{
		/*cur.dOctets = ((uint32_t*)(rec+fo.dOctets));
		cur.dPkts = ((uint32_t*)(rec+fo.dPkts));
		cur.srcaddr = ((uint32_t*)(rec+fo.srcaddr));
		cur.dstaddr = ((uint32_t*)(rec+fo.dstaddr));
		cur.dstport = ((uint16_t*)(rec+fo.dstport));
		cur.srcport = ((uint16_t*)(rec+fo.srcport));
		cur.prot = ((uint8_t*)(rec+fo.prot));
		fmt_ipv4(fmt_buf1, *cur.srcaddr, FMT_PAD_RIGHT);
		fmt_ipv4(fmt_buf2, *cur.dstaddr, FMT_PAD_RIGHT);*/

		rec = ftio_read(&ftio);
		if( !rec )
			break;

		if( fill_item_data(&tmp_item, rec, &fo) )
			tree_find_item(&tree_root, &tmp_item);


	}
	ftio_close(&ftio);


	print_recursive(&tree_root, current_timestamp, true);


	tree_free(&tree_root, false);

	return 0;
}
