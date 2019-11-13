#include "snappylib.h" // TODO: scrivere funzioni
#include <stdio.h>

#define SIZEFORTEST 1048576 //1MB


int main() {
    unsigned char *buffer;

    int a=16384;
    char literal_seq[a];

    unsigned char *data = getvarint_bindata(a);

    printf("Test for VARINT\n");

    printf("Data output for size %d in VARINT is:\n",
            a
            );

    printf("0x%x 0x%x 0x%x",*data, *(data+1), *(data+2));

    //testing 1 byte literal    OK
    a = 5;
    strncpy(literal_seq,
            "ciaoa",
            5);
    data = getliteral_bindata(literal_seq,a);
    printf("\n\nTest for literal lenght of %d, string is %s ", a, literal_seq);
    printf("\nOutput data is:\n0x%x\n", *data);

    for(int i=0; i<a; i++){
        printf("0x%x\n", *(data+i));
    }

    //testing 1+1 bytes literal OK
    a = 61;
    strncpy(literal_seq,
            "1234567890123456789012345678901234567890123456789012345678901123",
            61);
    data = getliteral_bindata(literal_seq,a);
    printf("\n\nTest for literal lenght of %d, string is %s ", a, literal_seq);
    printf("\nOutput data is:\n0x%x\n0x%x\n",
            *data,
            *(data+1));

    for(int i=0; i<a; i++){
        printf("0x%x\n", *(data+2+i));
    }

    //testing 1+3 bytes literal OK
    a = 65536;
    data = getliteral_bindata(literal_seq,a);
    printf("\n\nTest for literal lenght of %d, string is %s ", a, literal_seq);
    printf("\nOutput data is:\n0x%x\n0x%x\n0x%x\n0x%x\n",
           *data,
           *(data+1),
           *(data+2),
           *(data+3));

    for(int i=0; i<a; i++){
        printf("0x%x\n", *(data+4+i));
    }
}