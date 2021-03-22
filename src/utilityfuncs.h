//
// Created by Luca Pasini on 11/27/19.
// Made by : Luca Pasini
//

#ifndef SNAPPY_UTILITYFUNCS_H
#define SNAPPY_UTILITYFUNCS_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define max(n1, n2) (n1>n2 ? n1:n2)

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

/**
 * @param x
 *      integer argument to the logatithm
 * @return
 *      The logarithm in base 2, rounded down, of x
 */
_ulong log2_floor(_ulong x);


/**
 * @param s
 *      a 4 char string that need to be converted to uint
 * @return
 *      the uint corresponding "bitwise" to the char sequence
 */
_uint char_seq_to_uint(unsigned char s[], _uint len);

/**
 * @param fp is the file pointer of which we want to know the size
 * @return size of the file
 */
_size_t file_size(FILE *fp);

/**
 * Adds a character to a given string (binary compatible)
 * @param str: the str we want to add a char
 * @param initlen: the length of the input string
 * @param c: a pointer to the char to add
 * @return pointer to the str with the new char added
 */
char *add_char(char *str, _uint initlen, char *c);

#endif //SNAPPY_UTILITYFUNCS_H
