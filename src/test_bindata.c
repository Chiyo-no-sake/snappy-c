#include "snappy_algo.h"
#include "hashtable.h"
#include "utilityfuncs.h"


void print_datastream(unsigned char *buffer, int bytecount){
    printf("Data output buffer (%d Bytes) is:\n",bytecount);
    printf("%-8s|%8s\n", "hex", "char");

    for(int i=0; i<bytecount; i++){
        printf("0x%-6x|%8c\n", *(buffer+i), *(buffer+i));
    }
}

int main() {
    int a=16384;
    char literal_seq[BLOCK_SIZE_v1_1];

    unsigned char *data_literal;
    unsigned char *data_match;
    unsigned char *data_varint = getvarint_bindata(a);
    unsigned char *out_buffer;

    printf("Test for VARINT\n");
    int varint_usedbytes = get_lastusedbytes();
    print_datastream(data_varint, varint_usedbytes);

    //testing 1+1 bytes literal OK
    a = 60;
    strncpy(literal_seq,
            "123456789012345678901234567890"
            "123456789012345678901234567890",
            60);

    data_literal = getliteral_bindata(literal_seq,a);
    printf("\n\nTest for literal length of %d, string is %s \n", a, literal_seq);
    int literal_used_bytes = get_lastusedbytes();
    print_datastream(data_literal, literal_used_bytes);


    data_match = getmatch_bindata(65000, 120);
    printf("\nTest for match of length 120 and offset 65000\n");
    int match_used_bytes = get_lastusedbytes();
    print_datastream(data_match, get_lastusedbytes());

    printf("\nTest for concatenated data:\n");

    out_buffer = malloc(get_total_out_size(0));
    memcpy(out_buffer, data_varint, varint_usedbytes);
    memcpy(out_buffer+varint_usedbytes, data_literal, literal_used_bytes);
    memcpy(out_buffer+literal_used_bytes+varint_usedbytes, data_match, match_used_bytes);

    print_datastream(out_buffer, get_total_out_size(0));

    return 0;
}

