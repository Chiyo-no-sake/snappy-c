#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <bits/types/FILE.h>

bool writeFile(int size, char path[]);

unsigned char *getliteral_bindata(char *literal_seq, int seq_len);  //STABLE
unsigned char *getvarint_bindata(unsigned long size);               //STABLE
unsigned char *getmatch_bindata(int offset, int length);
int howmanydigits(unsigned long int val);                           //STABLE
unsigned char *dtobin(unsigned long int val);                       //STABLE

void setbyte(unsigned char *byte, char *bin_str);                   //STABLE


// -------- functions -------------


//set the passed buffer *byte to the string of '01' passed as *bin_str
void setbyte(unsigned char *byte, char *bin_str) {
    *byte = strtol(bin_str, 0, 2);
}

//Return a buffer containing the size passed in format of varint ready to be writed in 
// the output file + the string terminator
unsigned char *getvarint_bindata(unsigned long size) {
    unsigned char *buffer = malloc(5);     //cursor to move through the string
    unsigned char *out = buffer;                //output buffer

    unsigned short used_bytes;                  //byte used to write in base 2
    unsigned char *bin_size = malloc(35);                    //total size expressed in binary
    unsigned char byte_str[9];                  //string to simulate a byte in binary ("01")

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

    //Temporary buffer containing final result
    unsigned char temp_result_buffer[used_bytes];

    int i;
    for(i=0; i < 7-(howmanydigits(size)%7); i++) {
        //fill remaining bits to reach 7 multiple
        bin_size[i] = '0';
    }
    //convert to binary string the size
    strncpy(bin_size+i,dtobin(size),howmanydigits(size));

    //set values one byte at a time
    for (int i = 0; i < used_bytes; i++) {
        byte_str[0] = (i+1) < used_bytes ? '1' : '0';                     // add a 'continue?' bit
        strncpy((char *) byte_str+1,
                (char *) bin_size + ((used_bytes-i-1) * 7),
                7);                                                // write the 7 bits of the size
        byte_str[8] = '\0';                                                   // add str terminator
        setbyte(temp_result_buffer+i, (char *) byte_str);          // set bytes in the temp buffer
    }

    //write into the final output buffer + string terminator
    memcpy(buffer, temp_result_buffer, used_bytes);
    buffer[used_bytes] = '\0';

    return out;
}

//calculate neccessary digits for a conversion from base 10 to binary
int howmanydigits(unsigned long int val){
    unsigned long int cont=0;
    unsigned long int res = val;

    while(res > 0){
        res /= 2;
        cont++;
    }

    return cont;
}

//convert a decimal value to binary ( str of '01' ), with str terminator
unsigned char *dtobin(unsigned long int val){
    unsigned long int len=howmanydigits(val);
    unsigned char *buff = malloc(len);
    unsigned char *out = buff;

    unsigned long int res = val;

    while (res != 0) {
        *buff = (char) (48 + (res % 2)); //48+0 is '0' 48+1 is '1'
        res = (int) res / 2;
        buff++;
    }
    *buff = '\0';

    buff = out;

    for(int i=0; i<(int)len/2; i++){
        unsigned char tmp = *(buff+i);
        *(buff+i) = *(buff+len-i-1);
        *(buff+len-i-1) = tmp;
    }

    return out;

}

//return a buffer contaning the code corresponding a literal sequence ready to be written in file + the str terminator
unsigned char *getliteral_bindata(char literal_seq[], int seq_len) {
    int extrabytes;
    int binlen = howmanydigits(seq_len); //number of digits needed to represent seq_len in base 2

    unsigned char *seq_len_bin = dtobin(seq_len); //string representing the length in binary

    unsigned char *out_buffer;
    unsigned char *indexer; //to put right after the length in the buffer

    bool isLittle=0;

    char tag_byte[9];
    // Writing sequence length in buffer

    if(seq_len < 60){
        isLittle = 1;
        extrabytes=0;
        //1 byte for the tagbyte, 1 byte for the line terminator and 1 byte for each char
        out_buffer=malloc(2+extrabytes+seq_len);

        int i;
        for(i=0; i< 6 - binlen; i++) {
            //fill remaining bits to reach 6
            tag_byte[i] = '0';
        }
        
        strncpy(tag_byte + i, seq_len_bin, binlen);    //copying lenght into the char buffer
        strncpy(tag_byte + 6, "00\0", 3);          //setting TB '00'

        setbyte(out_buffer, tag_byte);
        //Tag byte + len bytes are now ok
    }else if (seq_len < 256){           //1 more byte for the literal lenght
        extrabytes=1;
        strncpy(tag_byte, "11110000", 8);  //tag byte, 60 + '00'
        indexer = out_buffer+2;         // positioned right after the size byte
    }else if (seq_len < 65536){         //2 more bytes for the lit length
        extrabytes=2;
        strncpy(tag_byte, "11110100", 8);  //tag byte, 61 + '00'
    }else if (seq_len < 16777216){      //3 more bytes for the lit length
        extrabytes=3;
        strncpy(tag_byte, "11111000", 8);   //tag byte, 62 + '00'
    }else{ // 4 more bytes for the lit length
        extrabytes=4;
        strncpy(tag_byte, "11111100", 8);   //tagbyte, 63 + '00'
    }

    if(!isLittle) {
        out_buffer = malloc(2 + extrabytes + seq_len);

        char size_bytes[extrabytes][9];      //8bit+str_terminator x each extra byte
        char tmp_str[extrabytes * 8 + 1];

        int i;
        for (i = 0; i < (8 * extrabytes) - binlen; i++) { //filling missing 0's at the start of the string
            tmp_str[i] = '0';
        }

        strncpy(tmp_str + i, seq_len_bin, binlen); //writing sequence len in binary

        setbyte(out_buffer, tag_byte);                  //setting the TagByte

        for (i = 0; i < extrabytes; i++) {
            strncpy(size_bytes[i], tmp_str + (i * 8), 8); //splitting size in bytes
            size_bytes[i][8] = '\0';

            setbyte(out_buffer + i + 1, size_bytes[i]);     //setting each byte
        }
    }

    indexer = out_buffer + 1 + extrabytes;              //moving indexer

    //ready to write sequence at 'indexer'
    int i;
    for(i=0; i<seq_len; i++){
        *(indexer+i) = literal_seq[i];
    }

    indexer[i] = '\0';

    return out_buffer;
}

//TODO test
bool writeToFile(char* buffer, char path[], int buff_lenght){

    FILE *file = fopen(path, "wb");

    if(!ferror(file)){
        fwrite(buffer, sizeof(char), buff_lenght, file);
    }else{
        return false;
    }

    fclose(file);
    return true;
}
