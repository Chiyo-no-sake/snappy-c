//
// Created by kalu on 1/7/20.
//

#include "list.h"

list_node *list_add_ifnotin(list_node *list_head, char *str, _uint len, _uint pos) {
    if (list_head == NULL) {
        //first insertion case
        list_head = malloc(sizeof(list_node));
        list_head->str = malloc(len);
        memcpy(list_head->str, str, len);
        list_head->len = len;
        list_head->pos = pos;

        list_head->next = NULL;

        return list_head;
    }

    if(list_head->len == len){
        if (memcmp(list_head->str, str, len) == 0){
            // the two strings are exactly the same
            // if so, the one already in must be the oldest, so overwrite it

            list_head->pos = pos;
            return list_head;
        }
    }

    list_node *head = list_head;
    while (list_head->next != NULL) {
        //control if already present
        if (list_head->next->len == len) {
            //same len strings
            if (memcmp(list_head->next->str, str, len) == 0) {
                // the two strings are exactly the same
                // if so, the one already in must be the oldest, so overwrite it

                list_head->next->pos = pos;
                return head;
            }
        }
        list_head = list_head->next;
    }

    //insertion case
    list_head->next = malloc(sizeof(list_head));
    list_head->next->str = malloc(len);
    memcpy(list_head->next->str, str, len);
    list_head->next->len = len;
    list_head->next->pos = pos;

    list_head->next->next = NULL;

    return head;

}

list_node *list_getel(list_node *list_head, char *str_to_search, _uint strlen) {
    while (list_head != NULL) {
        if (list_head->len == strlen) {
            //same len strings
            if (memcmp(list_head->str, str_to_search, strlen) == 0) {
                // the two strings are exactly the same
                return list_head;
            }
        }
        list_head = list_head->next;
    }

    return NULL;
}

list_node *clear_list(list_node *list_head) {

    if (list_head == NULL) return NULL;

    if (list_head->next == NULL) {
        free(list_head);
        return NULL;
    }

    list_head->next = clear_list(list_head->next);
}