#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool compress(char *, char *);

bool writeFile(int size, char path[]);

unsigned char *getvarint_bindata(unsigned long size);   //STABLE
unsigned char *dtobin_lend(unsigned long int val);      //STABLE
void fillbuffer0(unsigned char *buffer);                //STABLE
void setbyte(unsigned char *byte, char *bin_str);       //STABLE


// -------- functions -------------


//Comprime il file e ritorna true per ok, false per errore
bool compress(char in_path[], char out_path[]) {

}

//Riempie il buffer di 0's
void fillbuffer0(unsigned char *buffer) {
    while (*buffer) {
        *buffer = '\0';
        buffer++;
    }
}

//Ritorna un buffer contenente i dati da scrivere nel file nel formato varint
unsigned char *getvarint_bindata(unsigned long size) {
    unsigned char *buffer = malloc(5);     //funziona da buffer e scorre la stringa
    unsigned char *out = buffer;                  //output buffer

    unsigned short used_bytes;                  //byte usati per scrivere in binario
    unsigned char *bin_size;                    //size totale espressa in binario
    unsigned char byte_str[9];                  //stringa per simulare 1 byte in binario

    if (size < 128) {
        used_bytes = 1;
    } else if (size < 16384) {
        used_bytes = 2;
    } else if (size < 2097152) {
        used_bytes = 3;
    } else if (size < 268435456) {
        used_bytes = 4;
    } else {
        used_bytes = 5;
    }

    //buffer temporaneo contenitore del risultato finale
    unsigned char temp_result_buffer[used_bytes];

    //converte in stringa di binari la dimensione
    bin_size = dtobin_lend(size);

    //setta uno per volta i valori dei byte nel buffer
    for (int i = used_bytes; i > 0; i--) {
        strncpy((char *) byte_str, (char *) bin_size + ((i - 1) * 7),
                7);      // scrive nella stringa temporanea i 7 bit presi dalla bin_size alla giusta posizione
        byte_str[7] = i < used_bytes ? '1' : '0';                              // aggiugne bit per il continuo
        byte_str[8] = '\0';                                                   // terminatore
        setbyte(temp_result_buffer + (i - 1), (char *) byte_str);          // setta i byte nel buffer binario temporaneo
    }

    //riporta nel parametro il buffer binario
    memcpy(buffer, temp_result_buffer, used_bytes);
    buffer[used_bytes] = '\0';

    return out;
}

//decimal to binary little endian - tested OK
unsigned char *dtobin_lend(unsigned long int val) {
    unsigned char *buff = malloc(5);
    unsigned char *out = buff;

    unsigned long int res = val;
    short len = 0;

    while (res != 0) {
        *buff = (char) (48 + (res % 2));
        res = (int) res / 2;
        buff++;
        len++;
    };

    //Aggiunge 0 per tornare  un multiplo di 7
    while (len % 7 != 0) {
        len++;
        *buff = '0';
        buff++;
    }
    *buff = '\0';

    return out;
}

//set *byte to the string passed as *bin_str decimally
void setbyte(unsigned char *byte, char *bin_str) {
    *byte = strtol(bin_str, 0, 2);
}
