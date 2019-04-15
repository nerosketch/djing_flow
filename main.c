#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <ftlib.h>
#include "mybinarytree.h"


#define FLOW_COLS        8
#define uint             unsigned int



/*
 * Максиальная длина имени пользователя в бд
 */
#ifndef USERNAME_MAX_LENGTH
#define USERNAME_MAX_LENGTH                 127
#endif

typedef struct _IP_USER_TABLE{
    uint uid;
    uint32_t ip;
} IP_USER_TABLE;


/*
 * Формат файла:
 * ip-userid\n
 * ip в формате uint32_t
 */
IP_USER_TABLE **load_user_ip_table(FILE* p_table_file, uint *p_elems_lem)
{
    uint elem_len = 0;

    const size_t tbllen = sizeof(IP_USER_TABLE);

    if(fscanf(p_table_file, "count: %u\n", &elem_len) == EOF)
    {
        printf("EOF\n");
        return NULL;
    }

    if(elem_len == 0)
    {
        fprintf(stderr, "file must contain 'count: N' in header\n");
        return NULL;
    }

    IP_USER_TABLE **res = malloc(elem_len * sizeof(IP_USER_TABLE*));

    for(uint n=0; n<elem_len; n++)
    {

        IP_USER_TABLE *p_new_item = (IP_USER_TABLE*)malloc(tbllen);
        memset(p_new_item, 0, tbllen);
        res[n] = p_new_item;

        if(fscanf(p_table_file, "%u-%u\n", &p_new_item->ip, &p_new_item->uid) == EOF)
        {
            printf("EOF\n");
            break;
        }

    }

    *p_elems_lem = elem_len;

    return res;
}


void free_user_ip_table(IP_USER_TABLE **p_ipusr, uint arr_lenght)
{
    for(uint n=0; n<arr_lenght; n++)
        if(p_ipusr[n] != NULL)
        {
            free(p_ipusr[n]);
            p_ipusr[n] = NULL;
        }
    free(p_ipusr);
}


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

    printf("(%ld,%u,%u,%u,%u)", current_timestamp, p_tree->uid,
                p_tree->ip,  p_tree->octets, p_tree->packets);

    if(first)
        printf(";\n");
    else
        printf(",\n");

}


bool fill_item_data(TREE_ELEMENT *p_item, const char *rec, struct fts3rec_offsets *fo, IP_USER_TABLE **p_ipusr_tbl, uint ipusr_len)
{

    uint32_t dst_ip = *((uint32_t*)(rec+fo->dstaddr));
    uint32_t src_ip = *((uint32_t*)(rec+fo->srcaddr));

    p_item->octets = *((uint32_t*)(rec+fo->dOctets));
    p_item->packets = *((uint32_t*)(rec+fo->dPkts));

    for(uint n=0; n<ipusr_len; n++)
    {
        const IP_USER_TABLE *p_ipusr_item = p_ipusr_tbl[n];
        if( p_ipusr_item->ip == dst_ip || p_ipusr_item->ip == src_ip )
            p_item->uid = p_ipusr_item->uid;
    }

    if( is_ip_in_local_net( src_ip ) )
        p_item->ip = src_ip;
    else  if( is_ip_in_local_net( dst_ip ) )
        p_item->ip = dst_ip;
    else
        return false;

    return true;
}



void out_update_cache_query(const TREE_ELEMENT *p_item, const time_t *p_current_timestamp)
{
    if(p_item->octets < 524288)
        return;
    printf(
    "INSERT INTO flowcache(abon_id, last_time, octets, packets) "
    "VALUES(%u, %ld, %u, %u) "
    "ON DUPLICATE KEY UPDATE last_time=%ld, octets=%u, packets=%u;\n",
            p_item->uid, *p_current_timestamp, p_item->octets, p_item->packets,
            *p_current_timestamp, p_item->octets, p_item->packets);
}


int main(int argc, char **argv)
{
    char table_name[19] = {0};
    struct ftio oftio;
    struct ftver ftv;
    char *rec;
    struct fts3rec_offsets fo;
    uint tbl_len = 0;

    FILE *mf = fopen(argv[1], "r");
    if (mf == NULL)
    {
        fprintf(stderr, "Error open file\n");
        return -1;
    }
    // Храним таблицу соответствия id пользователей и их ip
    IP_USER_TABLE **p_ipusr_tbl = load_user_ip_table(mf, &tbl_len);
    fclose(mf);
    if(p_ipusr_tbl == NULL)
        return 2;

    time_t current_timestamp = time(NULL);


    if (ftio_init(&oftio, 0, FT_IO_FLAG_READ) < 0)
        fterr_errx(1, "ftio_init(): failed");

    ftio_get_ver(&oftio, &ftv);
    fts3rec_compute_offsets(&fo, &ftv);
    if (ftio_check_xfield(&oftio, /*FT_XFIELD_UNIX_SECS| FT_XFIELD_UNIX_NSECS|*/
        FT_XFIELD_DPKTS | FT_XFIELD_DOCTETS | FT_XFIELD_SRCADDR | FT_XFIELD_DSTADDR |
        FT_XFIELD_SRCPORT | FT_XFIELD_DSTPORT | FT_XFIELD_PROT)) {
            fterr_warnx("Flow record missing required field for format.");
            return -1;
    }

    curtime(table_name, 19, "flowstat_%d%m%Y");

    printf("CREATE TABLE IF NOT EXISTS %s (\n", table_name);
    printf("`cur_time` INT(10) UNSIGNED NOT NULL,\n"
        "`abon_id` INT(11) UNSIGNED NOT NULL,\n"
        "`ip` INT(10) UNSIGNED NOT NULL,\n"
        "`octets` INT unsigned NOT NULL DEFAULT 0,\n"
        "`packets` INT unsigned NOT NULL DEFAULT 0\n"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;\n");


    printf("INSERT INTO %s(`cur_time`,`abon_id`,`ip`,`octets`,`packets`) VALUES\n", table_name);


    TREE_ELEMENT tmp_item;
    tree_init_tree(&tmp_item);

    TREE_ELEMENT tree_root;
    tree_init_tree(&tree_root);


    while(true)
    {
        rec = ftio_read(&oftio);
        if( !rec )
            break;

        if( fill_item_data(&tmp_item, rec, &fo, p_ipusr_tbl, tbl_len) )
        {
            memcpy(&tree_root, &tmp_item, sizeof(TREE_ELEMENT));
            break;
        }
    }


    while(true)
    {
        rec = ftio_read(&oftio);
        if( !rec )
            break;

        if( fill_item_data(&tmp_item, rec, &fo, p_ipusr_tbl, tbl_len) )
            tree_find_item(&tree_root, &tmp_item);

    }
    ftio_close(&oftio);

    print_recursive(&tree_root, current_timestamp, true);

    tree_bypass_leafs(&tree_root, &out_update_cache_query, &current_timestamp);

    tree_free(&tree_root, false);

    free_user_ip_table(p_ipusr_tbl, tbl_len);

    return 0;
}
