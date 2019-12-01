//
// Created by kalu on 11/26/19.
//

#include "utilityfuncs.h"

int log2_floor(int x){
    int n = x;
    int r;
    for(r=0;n>0;r++){
        n >>= 1 ;
    }
    return r;
}

uint char_seq_to_uint(unsigned char s[], uint len){
    /*
     * Detailed explaination:
     *  the method tries to convert a sequence of char into a 32 bit unsigned
     *  integer reading it in LE.
     *  this is done by setting 8 bit at a time in the final int.
     *  to make this work we use a mask with the byte setted and shift left of
     *  a number 'i' corresponding the byte we want to set.
     *  So the last char, will be the first byte of the uint,
     *  the last-1 char will be the second and so on.
     *
     *  ex.
     *      n = 00000000 00001010 00101011 00111001
     *
     *      we want to set the first byte to FF
     *
     *      n = n | (0xFF << (8 * 3))
     */

    uint n = 0;
    for(ushort i = 0; i<len; i++){
        n = n | ((uint)s[i])<<8*i;
    }
    return n;
}

int max(int n1, int n2){
    return n1>n2 ? n1:n2;
}