//
// Created by Luca Pasini on 11/27/19.
// Made by : Luca Pasini & Ivan Negri --> see function's comments for details
//
//



//TODO:
// divide ouput writing in blocks to avoid memory overload in case of big files.

//TODO remove:
static unsigned int counter = 0;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

#include "snappy_algo.h"
#include <math.h>

//Selecting the first version of the block compression will be a little faster but compress a little bit less
static int std_block_size = BLOCK_SIZE_v1_1; //or BLOCK_SIZE_v1
static int total_file_size = 0;
static int last_used_bytes;
static bool progbar_activated = 0;
static int using_struct = 0; //'0' for no struct, '1' is List
static bool verbose;

void set_options(bool V11, bool list, bool verb, bool progbar) {
    std_block_size = V11 ? BLOCK_SIZE_v1_1 : BLOCK_SIZE_v1;
    using_struct = list ? 1 : 0;
    verbose = verb;
    progbar_activated = progbar;
}

/**
 * function used to display the progression bar and the speed
 * to avoid performance loss, we print it every 3 times the function is called
 * @param nbytes the bytes we arrived at in src file
 * @param srcsz the size of the src file in bytes
 *
 */
void print_progbar(_uint nbytes, _ulong srcsz) {
    static bool to_print = 1;
    static int skip_count = 0;
    if (nbytes == srcsz) to_print = 1;
    if (to_print) {
        static bool update_speed = false;
        static time_t last_timed = 0;
        static _uint last_nbytes;
        static float speed;

        time_t now = clock();
        _uint byte_diff = nbytes - last_nbytes;
        time_t time_diff = now - last_timed;

        update_speed = byte_diff != 0;

        if (update_speed && nbytes != srcsz)
            speed = (float) ((float) byte_diff) / ((float) time_diff / CLOCKS_PER_SEC) / 1048576;

        char *buf = malloc(50);
        char *buf2 = malloc(50);
        memset(buf, '#', 50);
        memset(buf2, '-', 50);

        float perc = (((float) nbytes / (float) srcsz) * 100);
        if (perc > 100) perc = 100;

        fprintf(stdout, "\r [%.*s%.*s]%5.1f%%\t %.1f MB/s", (int) perc >> 1, buf, (int) (100 - perc) >> 1, buf2, perc,
                speed);
        fflush(stdout);

        free(buf);
        free(buf2);

        last_nbytes = nbytes;
        last_timed = now;
        to_print = 0;
    } else {
        skip_count++;
        if (skip_count == 3) {
            to_print = 1;
            skip_count = 0;
        }
    }
}

bool compress(char *src_path, char *dest_path) {
    // Heuristic match skipping: If 32 bytes are scanned with no matches
    // found, start looking only at every other byte. If 32 more bytes are
    // scanned (or skipped), look at every third byte, etc.. When a match is
    // found, immediately go back to looking at every byte.
    // this variable denotes how many bits to skip (i.e. >>5, divided by 32)
    _uint bytes_without_match = 0;
    //used to keep track of the reached position in ouput buffer
    unsigned char *indexer;

    unsigned char *output = NULL;

    //variable for varint type
    unsigned char *varint_data;
    _size_t varint_size;

    //open file
    FILE *src_file = fopen(src_path, "rb");
    if (ferror(src_file)) {
        perror("Error opening file.");
        return 1;
    }
    _size_t src_size = file_size(src_file);

    FILE *dst_file = fopen(dest_path, "wb");

    if (verbose) {
        printf("File opened.\n");
        printf("File size is: %lu B\n", src_size);
    }

    //Output and other setup
    unsigned char *varint_output = malloc(5);
    unsigned char *block_output;

    //calculate varint
    varint_data = getvarint_bindata(src_size);
    varint_size = get_lastusedbytes;
    memcpy(varint_output, varint_data, varint_size);
    free(varint_data);

    //wrote varint
    fclose(dst_file);
    output = malloc(get_maxcompr_size(src_size));
    memcpy(output, varint_output, varint_size);

    if (verbose) {
        printf("VARINT done.\n");
        printf("VARINT is:\n");
        for (int i = 0; i < varint_size; ++i) {
            printf("%x\t\t%c\n", *(varint_output + i), *(varint_output + i));
        }
    }

    //setup useful vars
    _size_t left_bytes = src_size;
    _size_t cur_block_size;
    _uint block_num = 0;
    _uint byte_cnt = 0;

    indexer = output + varint_size;
    char *block = malloc(get_block_size(left_bytes));

    while ((int) left_bytes > 0) {
        //read 1 block
        if (progbar_activated)
            print_progbar(byte_cnt, src_size);

        ++block_num;
        cur_block_size = get_block_size(left_bytes);

        varint_size = 0;

        if (verbose)
            printf("Reading block %d (%lu B)\n", block_num, cur_block_size);

        //Hash table setup
        _size_t table_size = calculate_hashmap_size(cur_block_size);
        set_size(table_size);

        void *table;
        if (using_struct == 0) {
            //No struct
            setup_hashtable();
        } else {
            //linear list
            setup_hashtable_l();
        }

        if (verbose) {
            printf("Using Hash: %d\n", using_struct);
            printf("Hash table size set to: %lu\n", table_size);
        }

        //we need to read a full block
        fread(block, cur_block_size, 1, src_file);

        //end index and start index of the seq to analize
        char *s_i = block, *e_i = block + 3;

        char *possible_literal = malloc(1);
        _uint poss_lit_lenght = 0;

        //this reads the block if it is enough large
        if ((int) cur_block_size > 15)
            //control if less than 15 bits are left
            while (s_i + 15 < cur_block_size + block) {

                //write the sequence we are reading, the seq is from s_i and is long (e_i+1)-s_i
                if (verbose)
                    printf("Reading sequence: %.*s\n", (int) (e_i + 1 - s_i), s_i);
                //Is the sequence already in the table?
                void *possible_match = NULL;
                void *certain_match = NULL;

                _uint poss_hashcode;
                _uint len = e_i + 1 - s_i;

                poss_hashcode = gethash(s_i, len);

                if (using_struct == 0) {
                    //No struct
                    possible_match = (hash_entry *) hash_getel(s_i, len, poss_hashcode);
                } else {
                    //linear list
                    possible_match = (list_node *) hash_getel_l(s_i, len, poss_hashcode);
                }

                if (possible_match) {
                    //yes it is - MATCH FOUND - so:
                    //write the literal sequence found in the output
                    //and empty it
                    //then search a longer match, saving len and pos
                    //also reset skipbit variables

                    //resetting skipbits variables

                    bytes_without_match = 0;

                    if (verbose) {
                        if (using_struct == 0)
                            printf("Found a match: %s\n\tOFF/POS = %lu/%u\n",
                                   ((hash_entry *) possible_match)->str,
                                   (s_i - block) - ((hash_entry *) possible_match)->pos,
                                   ((hash_entry *) possible_match)->pos);
                        if (using_struct == 1)
                            printf("Found a match: %s\n\tOFF/POS = %lu/%u\n",
                                   ((list_node *) possible_match)->str,
                                   (s_i - block) - ((list_node *) possible_match)->pos,
                                   ((list_node *) possible_match)->pos);

                    }

                    if (poss_lit_lenght != 0) {
                        //writing literal
                        if (verbose)
                            printf("Writing literal: %.*s\n", poss_lit_lenght, possible_literal);

                        unsigned char *literal_data = getliteral_bindata(
                                possible_literal,
                                poss_lit_lenght);

                        _uint bytes_wrote = get_lastusedbytes;

                        if (verbose) {
                            printf("\tOutput is:\n");
                            for (int i = 0; i < bytes_wrote; ++i) {
                                printf("\t0x%x\t-\t%c\n", *(literal_data + i), *(literal_data + i));
                            }
                        }

                        memcpy(indexer, literal_data, bytes_wrote);
                        indexer = indexer + bytes_wrote;
                        //clean literal vars
                        free(literal_data);
                        poss_lit_lenght = 0;
                    } else if (verbose)
                        printf("No literal to write.\n");


                    if (verbose) printf("literal writing done.\n");


                    //trying more chars in match
                    while (possible_match && e_i < cur_block_size + 1 + block) {
                        if (verbose)
                            printf("Trying to find a longer match: ");

                        certain_match = possible_match;
                        ++e_i;

                        len = e_i + 1 - s_i;
                        poss_hashcode = gethash(s_i, len);

                        if (using_struct == 0) {
                            //No struct
                            possible_match = (hash_entry *) hash_getel(s_i, len, poss_hashcode);
                        } else {
                            //linear list
                            possible_match = (list_node *) hash_getel_l(s_i, len, poss_hashcode);
                        }

                        if (possible_match && verbose) {
                            if (using_struct == 0)
                                printf("Match Found: %.*s\n", ((hash_entry *) possible_match)->len,
                                       ((hash_entry *) possible_match)->str);
                            if (using_struct == 1)
                                printf("Match Found: %.*s\n", ((list_node *) possible_match)->len,
                                       ((list_node *) possible_match)->str);
                        }
                    }

                    //write last valid match on buffer
                    if (verbose) {
                        if (using_struct == 0)
                            printf("Writing Match: %s,\n OFF/LEN=%lu/%u\n", ((hash_entry *) certain_match)->str,
                                   s_i - block - ((hash_entry *) certain_match)->pos,
                                   ((hash_entry *) certain_match)->len);
                        if (using_struct == 1)
                            printf("Writing Match: %s,\n OFF/LEN=%lu/%u\n", ((list_node *) certain_match)->str,
                                   s_i - block - ((list_node *) certain_match)->pos,
                                   ((list_node *) certain_match)->len);
                    }

                    unsigned char *match_data = NULL;

                    if (using_struct == 0)
                        match_data = getmatch_bindata(s_i - block - ((hash_entry *) certain_match)->pos,
                                                      ((hash_entry *) certain_match)->len);
                    if (using_struct == 1)
                        match_data = getmatch_bindata(s_i - block - ((list_node *) certain_match)->pos,
                                                      ((list_node *) certain_match)->len);

                    _size_t used_B = get_lastusedbytes;

                    if (verbose) {
                        printf("\tOutput is:\n");
                        for (int i = 0; i < used_B; i++) {
                            printf("\t0x%x\t-\t%c\n", *(match_data + i), *(match_data + i));
                        }
                    }

                    memcpy(indexer, match_data, used_B);
                    indexer += used_B;

                    if (verbose)
                        printf("Match wrote with success.\n");

                    //adding possible_match that broke sequence to the table
                    //ATTENTION!
                    //need to add to the table after having wrote, so to make sure to not change
                    //values from the hashtable (same hashcode) before having wrote them !!
                    if (using_struct == 0)
                        hash_add(s_i, s_i - block, len, poss_hashcode);
                    if (using_struct == 1)
                        hash_addifnotin_l(s_i, s_i - block, len, poss_hashcode);

                    free(match_data);
                    byte_cnt = byte_cnt + e_i - s_i;
                    s_i = e_i;
                    e_i = s_i + 3;

                } else {
                    //no it is not, so
                    //add it and add literal then continue trying
                    if (using_struct == 0)
                        hash_add(s_i, s_i - block, len, poss_hashcode);
                    if (using_struct == 1)
                        hash_addifnotin_l(s_i, s_i - block, len, poss_hashcode);

                    possible_literal = add_char(possible_literal, poss_lit_lenght, s_i);
                    ++poss_lit_lenght;

                    //updating bits to skip
                    ++bytes_without_match;

                    _uint toskip = (bytes_without_match >> 5);
                    //if there are bytes to skip, add them to literal
                    for (int i = 0; i < toskip && e_i - block < cur_block_size; i++) {
                        if (verbose)
                            printf("Skipping a byte\n");
                        ++s_i;
                        ++e_i;
                        ++byte_cnt;
                        possible_literal = add_char(possible_literal, poss_lit_lenght, s_i);
                        ++poss_lit_lenght;
                    }
                    if (verbose)
                        printf("No match found.\nComposing literal: %.*s\n", poss_lit_lenght, possible_literal);

                    ++s_i;
                    ++e_i;
                    ++byte_cnt;
                }
            }

        //We are now near the blocks end, last 15 bytes are being written as literal, if there is any

        if (verbose)
            printf("\nLess than 15B are left. Writing as literal.\n");
        while (s_i - block < cur_block_size) {
            possible_literal = add_char(possible_literal, poss_lit_lenght, s_i);
            ++poss_lit_lenght;
            ++s_i;
            ++byte_cnt;
            if (verbose)
                printf("\tComposing literal: %.*s\n", poss_lit_lenght, possible_literal);
        }

        if (poss_lit_lenght > 0) {
            if (verbose)
                printf("\tWriting literal %.*s\n", poss_lit_lenght, possible_literal);
            unsigned char *literal_data = getliteral_bindata(
                    possible_literal,
                    poss_lit_lenght);

            _uint bytes_wrote = get_lastusedbytes;
            memcpy(indexer, literal_data, bytes_wrote);
            indexer = indexer + bytes_wrote;
            if (verbose)
                printf("\tliteral wrote\n");

            free(literal_data);
            free(possible_literal);
        }

        left_bytes = left_bytes - cur_block_size;

        if (verbose)
            printf("\nClearing data:\n");

        if (using_struct == 0)
            clear_hashtable();
        if (using_struct == 1)
            clear_table_l();

        if (verbose)
            printf("\tTable cleared\n");

    }

    free(block);
    if (verbose)
        printf("\tBlock cleared\n");

    if (verbose)
        printf("Writing to file: %s\n", dest_path);

    if (write_file(output, dest_path, get_total_out_size)) {
        if (verbose)
            printf("Correctly wrote block.\nInitial size: %f kB, Final size: %f kB\n", (float) src_size / 1024,
                   (float) get_total_out_size / 1024);
        free(output);
        fclose(src_file);
        if (progbar_activated) print_progbar(src_size, src_size);
        return true;
    } else {
        free(output);
        fclose(src_file);
        if (progbar_activated) print_progbar(src_size, src_size);
        return false;
    }

}

bool decompress(char *src_path, char *dest_path) {
    unsigned char *output;

    //buffer temporaneo per leggere 1 byte
    unsigned char *save_varint = malloc(1);

    /*
     * stringa per simulare i bit del varint
     */
    char buffer_varint[36];
    buffer_varint[35] = '\0';
    for (int i = 0; i < 35; ++i) {
        // il cilco riempe di 0 la stringa, cosìcchè se i byte letti sono meno di 35, potremo comunque convertire normalmente
        // la stringa binaria in decimale.
        // siamo così sicuri che se ad esempio vengano letti solo 7 bit, saranno sempre preceduti da soli 0.
        buffer_varint[i] = '0';
    }


    unsigned char *p_varint;

    //open file
    FILE *src_file = fopen(src_path, "rb");
    if (!src_file) return 0;

    //print file opened if verbose is true
    if (verbose)
        printf("File opened.\n");

    int verifica = 1;
    unsigned int dim_blocco = 0;
    //lettura varint dal file
    while (verifica == 1) {

        if (fread(save_varint, 1, 1, src_file) != 1) {
            perror("Read varint error.\n");
        }

        //converting bindata to binary string
        p_varint = dtobin((int) (*save_varint), 8);
        verifica = (*p_varint == '0') ? 0 : 1;

        //buffer_varint + 28 indica l'inizio dell'ultimo 'slot' in cui scriveremo i 7 bit alla fine del buffer. 7 poszioni prima ci sarà il penultimo e così via.
        strncpy((char *) buffer_varint + 28 - dim_blocco, (char *) p_varint + 1, 7);
        free(p_varint);
        dim_blocco = dim_blocco + 7;
    }

    if (verbose)
        printf("\nString varint to convert: %s\n", buffer_varint);

    _size_t filesize = strtol((char *) buffer_varint, NULL, 2);

    if (verbose)
        printf("Initial file size: %lu\n", filesize);


    //VARINT done.
    //Alloc output.
    output = malloc(filesize);

    unsigned char *indexer = output;

    //Start reading tagbytes
    char ch = (char) fgetc(src_file); //byte currently reading
    while (ch != EOF) {
        //Idle State - Waiting a TB
        //tagByte selection

        unsigned char byte = (unsigned char) ch;
        short cs = (short) (byte & 3); //case selecor

        if (verbose)
            printf("Reading byte: 0x%x\t-\t%c\n", byte, byte);

        if (cs == 0) {
            //it's a literal '00'
            if (verbose)
                printf("It's a literal\n");
            _uint len = (byte >> 2) + 1; //get first 6 bits + 1 for the len
            int extrabytes = (int) len - 60;
            if (extrabytes <= 0) {
                //the len is ok
                //read chars and write to output
                if (verbose)
                    printf("\tLength is %d\n", len);

            } else {
                //will read the len in more bytes
                unsigned char *ch_len = malloc(extrabytes);
                for (int i = 0; i < extrabytes; i++) {
                    *(ch_len + i) = fgetc(src_file);
                }

                len = char_seq_to_uint(ch_len, extrabytes);
                free(ch_len);
                if (verbose)
                    printf("\tLength is %d\n", len);
            }

            unsigned char *lit_out = malloc(len);
            for (_uint i = 0; i < len; i++) {
                *(lit_out + i) = fgetc(src_file);
            }
            memcpy(indexer, lit_out, len);

            if (verbose) {
                printf("\tLiteral ok. output is: \n\t");
                for (_uint i = 0; i < len; i++) {
                    printf("%c", *(lit_out + i));
                }
                printf("\n");
            }

            indexer += len;
            counter += len;
            free(lit_out);

        } else {
            // It's a Match
            _uint offs, len, offbytes;
            if (cs == 1) {
                //it's a 1B Match '01'
                if (verbose) {
                    printf("It's 1B match\n");
                }

                unsigned char next_byte = fgetc(src_file);
                len = ((byte >> 2) & 0x07) + 4; // get the len in the median 3 bytes
                offs = (((int) byte & 0xE0) << 3) | next_byte; // compose offset

                if (verbose)
                    printf("\tLen: %u\n\tOff: %u\n", len, offs);

            } else {
                //it's a 2B Match '10' or a 4B Match '11'
                if (verbose)
                    printf("It's %dB match\n", cs == 2 ? 2 : 4);
                offbytes = (cs == 2 ? 2 : 4);
                len = (byte >> 2) + 1; // get len-1 from first six bits

                unsigned char *ch_offs = malloc(offbytes);
                for (int i = 0; i < offbytes; i++) {
                    *(ch_offs + i) = fgetc(src_file);
                }

                offs = char_seq_to_uint(ch_offs, offbytes);
                free(ch_offs);

                if (verbose)
                    printf("\tLen: %u\n\tOff: %u\n", len, offs);
            }

            report_match(indexer, offs, len);
            indexer += len;
            counter += len;

            if (verbose) {
                printf("\tWrote the match:\n\t");
                for (int i = 0; i < len; i++) {
                    printf("%c", *(indexer - len + i));
                }
                printf("\n");
            }
        }
        ch = (char) fgetc(src_file);
    }

    if (verbose)
        printf("Reached EOF.\n");

    fclose(src_file);

    FILE *f_out = fopen(dest_path, "wb");
    fwrite(output, filesize, 1, f_out);
    fclose(f_out);

    free(output);
    free(save_varint);

    if (file_size(fopen(dest_path, "rb")) != filesize) {
        perror("SRC File format error. The file is corrupted.\n");
        return false;
    } else {
        if (verbose)
            printf("File decompressed successfully\n");
        return true;
    }
}

void report_match(unsigned char *actual_pos, _uint offs, _uint len) {
    unsigned char *start_pos = actual_pos - offs;
    for (_uint i = 0; i < len; ++i) {
        *(actual_pos + i) = *(start_pos + i);
    }
}

bool write_file(unsigned char *buffer, char *path, _size_t buff_length) {

    FILE *file = fopen(path, "wb");

    if (!ferror(file)) {
        fwrite(buffer, sizeof(unsigned char), buff_length, file);
    } else {
        perror("Error opening file.");
        return false;
    }

    fclose(file);
    return true;
}

//set the passed buffer *byte to the string of '01' passed as *bin_str
//first 8 chars of str will be only used
inline void setbyte(unsigned char *byte, unsigned char *bin_str) {
    *byte = 0;
    for (int i = 0; i < 8; ++i) {
        *byte = (bin_str[i] == '0' ? 0 : 1) << (7 - i) | *byte;
    }
}


unsigned char *getvarint_bindata(const _size_t srcsz) {
    last_used_bytes = 0;
    _size_t size = srcsz;

    unsigned short used_bytes;                  //byte used to write in base 2
    unsigned char *bin_size = malloc(35);                    //total size expressed in binary
    unsigned char byte_str[8];                  //string to simulate a byte in binary ("01")

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

    total_file_size = total_file_size + used_bytes;
    last_used_bytes = last_used_bytes + used_bytes;


    //Temporary buffer containing final result
    unsigned char temp_result_buffer[used_bytes];

    _uint digits = howmanydigits(size);
    _uint missing_bits = digits == 7 ? 0 : 7 - (digits % 7);
    _uint i;
    for (i = 0; i < missing_bits; ++i) {
        //fill remaining bits to reach 7 multiple
        bin_size[i] = '0';
    }
    //convert to binary string the size

    char *bin_str_tmp = (char *) dtobin(size, digits);
    memcpy((char *) bin_size + i, bin_str_tmp, digits);

    //set values one byte at a time
    for (i = 0; i < used_bytes; ++i) {
        byte_str[0] = (i + 1) < used_bytes ? '1' : '0';                     // add a 'continue?' bit
        memcpy((char *) byte_str + 1, (char *) bin_size + ((used_bytes - i - 1) * 7),
               7);     // write the 7 bits of the size
        setbyte(temp_result_buffer + i, byte_str);          // set bytes in the temp buffer
    }

    //write into the final output buffer + string terminator
    memcpy(buffer, temp_result_buffer, used_bytes);

    free(bin_size);
    free(bin_str_tmp);
    return out;
}


inline unsigned char *dtobin(unsigned long int val, _uint len) {
    unsigned char *buff = malloc(len);
    int rem;

    int i;
    for (i = 0; val != 0; ++i) {
        buff[len - 1 - i] = val & 1 ? '1' : '0';
        val >>= 1; // '/2'
    }

    rem = (int) len - i;

    for (i = 0; i < rem; ++i) {
        buff[i] = '0';
    }

    return buff;
}


//return a buffer contaning the code corresponding a literal sequence ready to be written in file
unsigned char *getliteral_bindata(const char literal_seq[], _uint seq_len) {
    last_used_bytes = 0;
    int extrabytes;
    unsigned char *out_buffer = NULL;
    unsigned char *indexer; //to put right after the length in the buffer
    bool isLittle = 0;

    // Writing sequence length in buffer

    if (seq_len < 60) {

        isLittle = 1;
        extrabytes = 0;

        //1 byte for the tagbyte +1 byte for each char
        out_buffer = malloc(1 + seq_len);

        out_buffer[0] = (seq_len - 1) << 2; //takes len-1 into the first 6 bits

        //Tag byte + len bytes are now ok
    } else if (seq_len < 256) {           //1 more byte for the literal lenght
        extrabytes = 1;
        out_buffer = malloc(1 + extrabytes + seq_len);
        out_buffer[0] = 0xf0;               //tag byte, 60 + '00'
    } else if (seq_len < 65536) {         //2 more bytes for the lit length
        extrabytes = 2;
        out_buffer = malloc(1 + extrabytes + seq_len);
        out_buffer[0] = 0xf4;              //tag byte, 61 + '00'
    } else if (seq_len < 16777216) {      //3 more bytes for the lit length
        extrabytes = 3;
        out_buffer = malloc(1 + extrabytes + seq_len);
        out_buffer[0] = 0xf8; //tag byte, 62 + '00'
    } else { // 4 more bytes for the lit length
        extrabytes = 4;
        out_buffer = malloc(1 + extrabytes + seq_len);
        out_buffer[0] = 0xfc; //tag byte 63+'00'
    }

    total_file_size = total_file_size + 1 + extrabytes + seq_len;
    last_used_bytes = 1 + extrabytes + seq_len;
    if (!isLittle) {
        //writing len to output
        memcpy(out_buffer + 1, &seq_len, extrabytes);
    }

    //moving indexer
    indexer = out_buffer + 1 + extrabytes;

    //ready to write sequence at 'indexer'
    memcpy(indexer, literal_seq, seq_len);

    return out_buffer;
}


unsigned char *getmatch_bindata(_uint offset, _uint length) {
    unsigned char *out_buffer;
    last_used_bytes = 0;

    if (length > 3 && length < 12 && offset < 2048) {
        // TESTED OK
        // 1 TB + 1 B
        // TagByte<0-2> => Most Significative Bit of Offset
        // TagByte<3-5> => Length - 4
        // TagByte<6-7> => '01'
        // Byte => Offset remaining bits (offset<3-10>

        out_buffer = malloc(2); //tB + 1B
        total_file_size = total_file_size + 2;
        last_used_bytes = last_used_bytes + 2;

        length -= 4;
        out_buffer[0] = ((offset >> 3) & 0xE0) | ((length) << 2) | 0x1; // take the first 3 bit of offset
        out_buffer[1] = (offset & 0x00FF);

    } else if (length < 65) {
        // TESTED OK
        // 1 TB + 2B ( + 2B )
        // TagByte<0-5> => lenght -1
        // TagByte<6-7> => '10' ( or '11')
        // Byte1+Byte2 ( +Byte3+Byte4 ) => offset Little Endian
        short int offset_bytes_used;
        int is2B = (offset <= 65535);

        if (is2B) {
            out_buffer = malloc(3); // 1 TagByte +2 B
            total_file_size = total_file_size + 3;
            last_used_bytes = last_used_bytes + 3;
            offset_bytes_used = 2;
        } else {
            out_buffer = malloc(5); // 1 TagByte +4 B
            total_file_size = total_file_size + 5;
            last_used_bytes = last_used_bytes + 5;
            offset_bytes_used = 4;
        }
        // calculating tagbyte

        length--;
        out_buffer[0] = (length << 2) | (is2B ? 0x2 : 0x3); // set last 2 bits of match tag
        memcpy(out_buffer + 1, &offset, offset_bytes_used); //set offset bits LE
    } else {
        //length bigger than 64 will be splitted in less long matches

        int match1_usedB, match2_usedB;
        int newlen1 = length >> 1;

        unsigned char *buff1 = getmatch_bindata(offset, newlen1);
        match1_usedB = get_lastusedbytes;
        unsigned char *buff2 = getmatch_bindata(offset, length - newlen1);
        match2_usedB = get_lastusedbytes;


        out_buffer = malloc(match1_usedB + match2_usedB);
        memcpy(out_buffer, buff1, match1_usedB);
        memcpy(out_buffer + match1_usedB, buff2, match2_usedB);

        last_used_bytes = match1_usedB + match2_usedB;
    }

    return out_buffer;
}

#pragma clang diagnostic pop