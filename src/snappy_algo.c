#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

#include "snappy_algo.h"
#include "utilityfuncs.h"

int total_file_size = 0;
int last_used_bytes;

//set the passed buffer *byte to the string of '01' passed as *bin_str
void setbyte(unsigned char *byte, unsigned char *bin_str) {
    *byte = strtol((char*)bin_str, 0, 2);
}


unsigned char *getvarint_bindata(unsigned long size) {
    last_used_bytes = 0;



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

    unsigned char *buffer = malloc(5);     //cursor to move through the string
    unsigned char *out = buffer;                //output buffer

    total_file_size += used_bytes;
    last_used_bytes += used_bytes;


    //Temporary buffer containing final result
    unsigned char temp_result_buffer[used_bytes];

    unsigned int i;
    for(i=0; i < 7-(howmanydigits(size)%7); i++) {
        //fill remaining bits to reach 7 multiple
        bin_size[i] = '0';
    }
    //convert to binary string the size
    strncpy((char *)bin_size+i,(char *)dtobin(size),howmanydigits(size));

    //set values one byte at a time
    for (i = 0; i < used_bytes; i++) {
        byte_str[0] = (i+1) < used_bytes ? '1' : '0';                     // add a 'continue?' bit
        strncpy((char *) byte_str+1,
                (char *) bin_size + ((used_bytes-i-1) * 7),
                7);                                                // write the 7 bits of the size
        byte_str[8] = '\0';                                                   // add str terminator
        setbyte(temp_result_buffer+i, byte_str);          // set bytes in the temp buffer
    }

    //write into the final output buffer + string terminator
    memcpy(buffer, temp_result_buffer, used_bytes);

    return out;
}


unsigned int howmanydigits(unsigned long int val){
    return log2_floor((int)val);
}


unsigned char *dtobin(unsigned long int val){
    unsigned long int len=howmanydigits(val);
    unsigned char *buff = malloc(len);
    unsigned char *out = buff;

    unsigned long int res = val;

    while (res != 0) {
        *buff = res & 1 ? '1' : '0'; //48+0 is '0' 48+1 is '1'
        res >>= 1; // '/2'
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
unsigned char *getliteral_bindata(const char literal_seq[], int seq_len) {
    last_used_bytes = 0;
    int extrabytes;
    unsigned int binlen = howmanydigits(seq_len); //number of digits needed to represent seq_len in base 2
    unsigned char *seq_len_bin = dtobin(seq_len); //string representing the length in binary
    unsigned char *out_buffer=NULL;
    unsigned char *indexer; //to put right after the length in the buffer
    unsigned char tag_byte[9];
    bool isLittle=0;


    // Writing sequence length in buffer

    if(seq_len < 60){
        isLittle = 1;
        extrabytes=0;
        //1 byte for the tagbyte and 1 byte for each char
        out_buffer=malloc(1+extrabytes+seq_len);
        total_file_size += 1+extrabytes+seq_len; //str terminator wont be in file
        last_used_bytes += 1+extrabytes+seq_len;

        unsigned int i;
        for(i=0; i< 6 - binlen; i++) {
            //fill remaining bits to reach 6
            tag_byte[i] = '0';
        }

        strncpy((char *)tag_byte + i, (char *)seq_len_bin, binlen);    //copying lenght into the char buffer
        strncpy((char *)tag_byte + 6, "00\0", 3);          //setting TB '00'

        setbyte(out_buffer, tag_byte);
        //Tag byte + len bytes are now ok
    }else if (seq_len < 256){           //1 more byte for the literal lenght
        extrabytes=1;
        strncpy((char *)tag_byte, "11110000", 8);  //tag byte, 60 + '00'
    }else if (seq_len < 65536){         //2 more bytes for the lit length
        extrabytes=2;
        strncpy((char *)tag_byte, "11110100", 8);  //tag byte, 61 + '00'
    }else if (seq_len < 16777216){      //3 more bytes for the lit length
        extrabytes=3;
        strncpy((char *)tag_byte, "11111000", 8);   //tag byte, 62 + '00'
    }else{ // 4 more bytes for the lit length
        extrabytes=4;
        strncpy((char *)tag_byte, "11111100", 8);   //tagbyte, 63 + '00'
    }

    if(!isLittle) {
        out_buffer = malloc(1 + extrabytes + seq_len);
        total_file_size += 1+extrabytes+seq_len; //str terminator wont be in file
        last_used_bytes = 1+extrabytes+seq_len;

        unsigned char size_bytes[extrabytes][9];      //8bit+str_terminator x each extra byte
        unsigned char tmp_str[extrabytes * 8 + 1];

        unsigned int i;
        for (i = 0; i < (8 * extrabytes) - binlen; i++) { //filling missing 0's at the start of the string
            tmp_str[i] = '0';
        }

        strncpy((char *)tmp_str + i, (char *)seq_len_bin, binlen); //writing sequence len in binary
        setbyte(out_buffer, tag_byte);                  //setting the TagByte

        for (i = 0; i < extrabytes; i++) {
            strncpy((char *)size_bytes[i], (char *)tmp_str + (i * 8), 8); //splitting size in bytes
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

    return out_buffer;
}


unsigned char *getmatch_bindata(int offset, int length){
    unsigned char *out_buffer;
    last_used_bytes = 0;

    if(length>=4 && length <= 11 && offset <= 2047){
        // TESTED OK
        // 1 TB + 1 B
        // TagByte<0-2> => Most Significative Bit of Offset
        // TagByte<3-5> => Length - 4
        // TagByte<6-7> => '01'
        // Byte => Offset remaining bits (offset<4-10>

        out_buffer = malloc(2); //tB + 1B
        total_file_size += 2;
        last_used_bytes += 2;

        unsigned char tagByte_str[9];    //8 bits string simulator + 1 str terminator
        unsigned char byte_str[9];       //8 bits string simulator + 1 str terminator
        unsigned char len_str[3];
        unsigned char offset_str[10];

        //filling len, stretching to reach 3 bits
        unsigned char *temp_len_str = dtobin(length-4);
        unsigned int len_initlen = howmanydigits(length-4);
        unsigned int i;
        for(i=0;i<(3-len_initlen); i++){
            len_str[i] = '0';
        }
        strncpy((char *)len_str+i, (char *)temp_len_str, len_initlen);

        // ----- len_str now ready ---------

        //convert to decimal the offset and stretch to reach 10 bits
        unsigned char *temp_offset_str = dtobin(offset);
        unsigned int offset_initlen = howmanydigits(offset);
        for(i=0; i<(11-offset_initlen); i++){
            offset_str[i] = '0';
        }
        strncpy((char *)offset_str+i, (char *)temp_offset_str, offset_initlen);

        // ----- offset_str now ready -------

        //set tagByte
        strncpy((char *)tagByte_str, (char *)offset_str, 3);
        strncpy((char *)tagByte_str+3, (char *)len_str, 3);
        strncpy((char *)tagByte_str+6, "01", 2);
        tagByte_str[8] = '\0'; //add str terminator
        //done

        //set 2nd Byte
        strncpy((char *)byte_str, (char *)offset_str+3, 8);
        byte_str[8] = '\0';
        //done

        //set outbuffer
        setbyte(out_buffer,tagByte_str);
        setbyte(out_buffer+1, byte_str);

    }else if(length<=64){
        // TESTED OK
        // 1 TB + 2B ( + 2B )
        // TagByte<0-5> => lenght -1
        // TagByte<6-7> => '10'
        // Byte1+Byte2 ( +Byte3+Byte4 ) => offset Little Endian

        short int offset_bits_used;
        short int offset_bytes_used;

        if(offset<=65535) {
            out_buffer = malloc(3); // 1 TagByte +2 B
            total_file_size += 3;
            last_used_bytes += 3;
            offset_bits_used = 16;
            offset_bytes_used = 2;
        }else{
            out_buffer = malloc(5); // 1 TagByte +4 B
            total_file_size += 5;
            last_used_bytes += 5;
            offset_bits_used = 32;
            offset_bytes_used = 4;
        }

        unsigned char offset_bin_str[offset_bits_used];
        unsigned char offset_bytes[offset_bytes_used][9];

        // calculating tagbyte
        unsigned char tagByte_str[9];
        unsigned char *temp_len_str = dtobin(length-1);
        unsigned int bitcount = howmanydigits(length-1);

        unsigned int i;
        for(i=0; i<6-bitcount; i++){
            tagByte_str[i]='0';
        }

        strncpy((char*)tagByte_str+i, (char *)temp_len_str, bitcount);
        strncpy((char *)tagByte_str+6, "10", 2);
        tagByte_str[8]='\0';
        //-------TagByte ready ---------

        unsigned char *temp_off_string = dtobin(offset);
        bitcount = howmanydigits(offset);

        for(i=0;i<offset_bits_used-bitcount;i++){
            offset_bin_str[i]='0';
        }

        strncpy((char*)offset_bin_str+i, (char*)temp_off_string, bitcount);

        short int byte=offset_bytes_used-1;
        for(i=0;byte>=0;byte--){
            strncpy((char*)offset_bytes[byte], (char*)offset_bin_str+i, 8);
            offset_bytes[byte][8]='\0';
            i+=8;
        }
        //-------- 2 bytes ready ----------

        setbyte(out_buffer, tagByte_str);
        for(i=0;i<offset_bytes_used;i++){
            setbyte(out_buffer+1+i,offset_bytes[i]);
        }
    } else{
        //length bigger than 64 will be splitted in less long matches

        int match1_usedB, match2_usedB;
        int newlen1 = (int)(length/2);

        unsigned char *buff1 = getmatch_bindata(offset, newlen1);
        match1_usedB = get_lastusedbytes();
        unsigned char *buff2 = getmatch_bindata(offset, length-newlen1);
        match2_usedB = get_lastusedbytes();


        out_buffer = malloc(match1_usedB+match2_usedB);
        memcpy(out_buffer, buff1, match1_usedB);
        memcpy(out_buffer+match1_usedB, buff2, match2_usedB);

        last_used_bytes = match1_usedB+match2_usedB;
    }

    return out_buffer;
}

size_t get_block_size(size_t remaining_input){
    return (
            remaining_input<BLOCK_SIZE_v1_1 ?
            remaining_input :
            BLOCK_SIZE_v1_1
    );
}

size_t get_total_out_size() {
    return total_file_size;
}

size_t get_lastusedbytes(){
    return last_used_bytes;
}


//TODO test
bool write_file(unsigned char* buffer, char *path, int buff_length){

    FILE* file = fopen(path, "wb");

    if(!ferror(file)){
        fwrite(buffer, sizeof(unsigned char), buff_length, file);
    }else{
        return false;
    }

    fclose(file);
    return true;
}

#pragma clang diagnostic pop