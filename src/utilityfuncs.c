//
// Created by Luca Pasini on 11/27/19.
// Made by : Luca Pasini
//

#include "utilityfuncs.h"

inline _ulong log2_floor(_ulong x){
    _ulong n = x;
    _ulong r;
    for(r=0;n>0;r++){
        n >>= 1 ;
    }
    return r;
}

_uint char_seq_to_uint(unsigned char s[], _uint len){
    /*
     * Detailed explaination:
     *  the method tries to convert a sequence of char into a 32 bit unsigned
     *  integer reading it in LE.
     *  this is done by setting 8 bit at a time in the final int.
     *  to make this work we use a mask with the byte setted and shift left of
     *  a number 'i' corresponding the byte we want to set.
     *  So the last char, will be the first byte of the _uint,
     *  the last-1 char will be the second and so on.
     *
     *  ex.
     *      n = 00000000 00001010 00101011 00111001
     *
     *      we want to set the first byte to FF
     *
     *      n = n | (0xFF << (8 * 3))
     */

    _uint n = 0;
    for(_ushort i = 0; i<len; i++){
        n = n | ((_uint)s[i])<<(i<<3);
    }
    return n;
}



char *add_char(char *str, _uint initlen, char *c){
    char *new_str = malloc(initlen+2);

    memcpy(new_str,str,initlen);
    new_str[initlen] = c[0];

    free(str);
    return new_str;
}

inline _size_t file_size(FILE *fp){
    fseek(fp, 0L, SEEK_END);
    _size_t toret = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return toret;
}