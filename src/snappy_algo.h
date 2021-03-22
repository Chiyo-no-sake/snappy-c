//
// Created by Luca Pasini on 11/27/19.
// Made by : Luca Pasini & Ivan Negri --> see function's comments for details
//

#ifndef SNAPPY_ALGO_LIB_IMPORT
#define SNAPPY_ALGO_LIB_IMPORT

/**
 * Choose which version to use in snappy_algo.c on std_block_size
 * v1 -> lesser compression but little more speed
 * v1.1 -> more compressed output but little little slower
 */
#define BLOCK_SIZE_v1 32768
#define BLOCK_SIZE_v1_1 65536

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



//return the number of bytes used totally by all the calls of '_bindata()' functions
#define get_total_out_size total_file_size

//return the number of bytes used in the most recent call to a *_bindata() functions
#define get_lastusedbytes last_used_bytes

//return the block size that will be used given thhe size remaining in input
#define get_block_size(rem_in) (rem_in < std_block_size ? rem_in : std_block_size)

//calculate neccessary digits for a conversion from base 10 to binary
#define howmanydigits(val) (log2_floor(val))

#include "utilityfuncs.h"
#include "hashtable.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "list.h"
/*
 * made by PASINI LUCA
 * The functions of the "*_bindata" family will update a counter called last_used_bytes
 * that holds the number of bytes used by the buffer calculated in such functions.
 * This will allow to have always the size of the buffers, so we can combine them and
 * save to a file.
 *
 */

void set_options(bool V11, bool list, bool verb, bool progbar);

/**
 * made by PASINI LUCA
 * @param src_path
 * @param dest_path
 * @return
 */
bool compress(char *src_path, char *dest_path);

/**
 * made by NEGRI IVAN
 * @param src_path
 * @param dest_path
 * @return
 */
bool decompress(char *src_path, char *dest_path);

bool write_file(unsigned char *buffer, char *path, _size_t buff_length);

/**
 * made by NEGRI IVAN
 * @param
 * @return
 */
_size_t read_filesize(unsigned char *varint, int dim);

/**
 * made by PASINI LUCA
 * @param literal_seq the sequence to be encoded
 * @param seq_len the len of seq
 * @return the stream to be written in file
 */
unsigned char *getliteral_bindata(const char *literal_seq, _uint seq_len);

/**
 * made by PASINI LUCA
 * @param srcsz the src file size
 * @return the stream to be written in file
 */
unsigned char *getvarint_bindata(_size_t srcsz);

/**
 * made by PASINI LUCA
 * @param offset the offset of the match
 * @param length the length of the match
 * @return the stream to be written in file
 */
unsigned char *getmatch_bindata(_uint offset, _uint length);

/**
 * made by PASINI LUCA
 * @param val the value to convert
 * @return a string simulating the binary of val
 */
unsigned char *dtobin(unsigned long int val, _uint len);

/**
 * made by PASINI LUCA
 * @param byte the buffer where to write
 * @param bin_str a string simulating a binary byte, long 8 bits
 */
void setbyte(unsigned char *byte, unsigned char *bin_str);

/**
 * made by PASINI LUCA
 * @param actual_pos the position of the cursor writing in the output string
 * @param offs offset of the match
 * @param len length of the match
 */
void report_match(unsigned char *actual_pos, _uint offs, _uint len);


_size_t get_lastusedB();
_size_t get_total_out_sz();
#endif