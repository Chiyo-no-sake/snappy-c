//
// Created by kalu on 1/7/20.
//

#ifndef SNAPPY_LIST_H
#define SNAPPY_LIST_H


#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifndef _ulong
#define _ulong unsigned long
#endif

#ifndef _uint
#define _uint unsigned int
#endif

#ifndef _ushort
#define _ushort unsigned short
#endif

#ifndef _size_t
#define _size_t unsigned long
#endif

typedef struct listnode_type {
    struct listnode_type *next;

    _uint pos;
    _uint len;
    char *str;
}list_node;

list_node *list_add_ifnotin(list_node *list_head, char *str, _uint len, _uint pos);

list_node *list_getel(list_node *list_head, char *str_to_search, _uint strlen);

list_node *clear_list(list_node *list_head);


#endif //SNAPPY_LIST_H
