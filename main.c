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


#define FLOW_COLS		8
#define uint			unsigned int



/*
 * Проверяю что ip в локальной сети 10.0.0.0/8
 */
/*inline bool is_ip_in_local_net(uint32_t ip){
	return ip > 0xa000001 && ip < 0xafffffe;
}


uint32_t ip2int(const char* ip)
{
	uint32_t res = 0;
	inet_pton(AF_INET, ip, &res);
	return htonl(res);
}


uint str_split(char* str, const char* delimiter, char** pInChunks)
{
	char* dat = strtok(str, " ");
	register uint n=0;
	while(dat)
	{
		pInChunks[n++] = dat;
		dat = strtok(NULL, " ");
	}
	return n;
}*/


void curtime(char* pInStrTime, const uint maxlen)
{
	time_t rawtime;
	time( &rawtime );
	strftime(pInStrTime, maxlen, "flowstat_%d%m%Y", localtime( &rawtime ));
}


int main()
{
	//FILE* f = stdin;
	char table_name[19] = {0};
	struct ftio ftio;
	struct ftver ftv;
	char *rec;
	struct fts3rec_offsets fo;
	struct fts3rec_all cur;
	DJING_CONF_STRUCT conf;

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

	curtime(table_name, 19);

	printf("CREATE TABLE IF NOT EXISTS %s (\n", table_name);
	printf("`cur_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\n"
		"`src_ip` INT(10) UNSIGNED NOT NULL,\n"\
		"`dst_ip` INT(10) UNSIGNED NOT NULL,\n"\
		"`proto` smallint(2) unsigned NOT NULL DEFAULT 0,\n"\
		"`src_port` smallint(5) unsigned NOT NULL DEFAULT 0,\n"\
		"`dst_port` smallint(5) unsigned NOT NULL DEFAULT 0,\n"\
		"`octets` INT unsigned NOT NULL DEFAULT 0,\n"\
		"`packets` INT unsigned NOT NULL DEFAULT 0\n"\
		") ENGINE=MyISAM DEFAULT CHARSET=utf8;\n");


	printf("INSERT INTO %s(`src_ip`, `dst_ip`, `proto`, `src_port`, `dst_port`, `octets`, `packets`) VALUES\n", table_name);


	long long int line_counter=0;
	rec = ftio_read(&ftio);
	if(!rec){
		perror("ERROR: flow not read\n");
		ftio_close(&ftio);
		return -1;
	}
	while (true) {

		cur.dOctets = ((uint32_t*)(rec+fo.dOctets));
		cur.dPkts = ((uint32_t*)(rec+fo.dPkts));
		cur.srcaddr = ((uint32_t*)(rec+fo.srcaddr));
		cur.dstaddr = ((uint32_t*)(rec+fo.dstaddr));
		cur.srcport = ((uint16_t*)(rec+fo.srcport));
		cur.dstport = ((uint16_t*)(rec+fo.dstport));
		cur.prot = ((uint8_t*)(rec+fo.prot));
		/*fmt_ipv4(fmt_buf1, *cur.srcaddr, FMT_PAD_RIGHT);
		fmt_ipv4(fmt_buf2, *cur.dstaddr, FMT_PAD_RIGHT);*/

		printf("(%u,%u,%hu,%u,%u,%u,%u)",
			*cur.srcaddr, *cur.dstaddr, *cur.prot, *cur.srcport, *cur.dstport, *cur.dOctets, *cur.dPkts);


		if( ++line_counter >= conf.mysql_rows_per_request){
			printf(";\nINSERT INTO %s(`src_ip`, `dst_ip`, `proto`, `src_port`, `dst_port`, `octets`, `packets`) VALUES\n", table_name);
			line_counter = 0;
		}else{
			rec = ftio_read(&ftio);
			if(!rec )
				break;
			printf(",\n");
		}
	}
	ftio_close(&ftio);
	putc('\n', stdout);

	return 0;
}
