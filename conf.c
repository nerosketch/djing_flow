#include <libconfig.h>
#include <string.h>
#include <stdint.h>
#include "conf.h"


int read_config(DJING_CONF_STRUCT* p_conf_struct, const char* fname){
	config_t c;

	memset(p_conf_struct, 0, sizeof(DJING_CONF_STRUCT));
	memset(&c, 0, sizeof(config_t));

	config_init(&c);
	if (config_read_file(&c, fname) != CONFIG_TRUE){
		fprintf(stderr, "error read config file: %s\n", fname);
		config_destroy(&c);
		return E_FAIL;
	}

	{
		long long int mysql_rows_per_request = 0;
		if(config_lookup_int64(&c, "mysql_rows_per_request", &mysql_rows_per_request)){
			if(mysql_rows_per_request < 0)
				printf("invalid value: mysql_rows_per_request < 0\n");
			else{
				p_conf_struct->mysql_rows_per_request = mysql_rows_per_request;
			}
		}else
			printf("mysql_rows_per_request not found\n");
	}


	config_destroy(&c);
	return E_SUCCESS;
}
