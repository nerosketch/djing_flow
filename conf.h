/* 
 * File:   conf.h
 * Author: bashmak
 *
 * Created on 22 апреля 2017 г., 10:05
 */

#ifndef CONF_H
#define CONF_H


#ifdef __cplusplus
extern "C" {
#endif

#ifndef E_FAIL
#define E_FAIL                                          -1
#endif
#ifndef E_SUCCESS
#define E_SUCCESS                                  0
#endif
    

typedef struct _DJING_CONF_STRUCT{
    long long int mysql_rows_per_request;
} DJING_CONF_STRUCT;


/*
 * Читаем конфиг
 */
int read_config(DJING_CONF_STRUCT* p_conf_struct, const char* fname);



#ifdef __cplusplus
}
#endif

#endif /* CONF_H */

