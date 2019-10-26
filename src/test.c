#include "snappylib.h" // TODO: scrivere funzioni
#include <stdio.h>

#define SIZEFORTEST 1048576 //1MB


int main() {
    unsigned char *buffer;

    buffer = getvarint_bindata(SIZEFORTEST);

    printf("Original dimension was: %d Bytes aka %d Kb\n", SIZEFORTEST, SIZEFORTEST / 1024);

    printf("Output bytes:\n");
    for (int i = 0; i < 5; i++) {
        printf("%x\n", *(buffer + i));
    }

}