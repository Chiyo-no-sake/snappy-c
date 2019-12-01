//
// Created by kalu on 11/26/19.
//

#ifndef SNAPPY_UTILITYFUNCS_H
#define SNAPPY_UTILITYFUNCS_H
#include <stdlib.h>

/**
 * @param x
 *      integer argument to the logatithm
 * @return
 *      The logarithm in base 2, rounded down, of x
 */
int log2_floor(int x);


/**
 * @param s
 *      a 4 char string that need to be converted to uint
 * @return
 *      the uint corresponding "bitwise" to the char sequence
 */
uint char_seq_to_uint(unsigned char s[], uint len);

int max(int n1, int n2);

#endif //SNAPPY_UTILITYFUNCS_H
