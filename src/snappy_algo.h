#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <bits/types/FILE.h>

#define BLOCK_SIZE_v1 32768
#define BLOCK_SIZE_v1_1 65536

/*
 * The functions of the "*_bindata" family will update a counter called last_used_bytes
 * that holds the number of bytes used by the buffer calculated in such functions.
 * This will allow to have always the size of the buffers, so we can combine them and
 * save to a file.
 *
 */


bool write_file(unsigned char* buffer, char *path, int buff_length);

//return a buffer contaning the data corresponding a literal sequence ready to be written
//in file
unsigned char *getliteral_bindata(const char *literal_seq, int seq_len);  //STABLE

//return a buffer contaning the data corresponding the VARINT to be written
//in file
unsigned char *getvarint_bindata(unsigned long size);               //STABLE

//return a buffer contaning the data corresponding a match ready to be written
//in file
unsigned char *getmatch_bindata(int offset, int length);            //IN PROGRESS

//convert a decimal value (val) to binary
//(val need to be str of '01' with str terminator)
unsigned char *dtobin(unsigned long int val);                       //STABLE

//calculate neccessary digits for a conversion from base 10 to binary
unsigned int howmanydigits(unsigned long int val);                           //STABLE

//set the passed buffer *byte to the string of '01' passed as *bin_str
//NEED STR TERMINATOR ('\0') in bin_str
void setbyte(unsigned char *byte, unsigned char *bin_str);                   //STABLE

//return the number of bytes used in the most recent call to a *_bindata() function
size_t get_lastusedbytes();

//return the number of bytes used totally by all the calls of '_bindata()' functions
size_t get_total_out_size();

size_t get_block_size(size_t remaining_input);

//
