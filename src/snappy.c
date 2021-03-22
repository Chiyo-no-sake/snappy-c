//
// Created by Luca Pasini on 11/27/19.
// Made by : Luca Pasini & Ivan Negri
//

/*
 * Main program for snappy compression
 * Usage:
 *  snappy -[c|d] <srcfile> <destfile>
 */

#include <stdio.h>
#include "snappy_algo.h"
#include "time.h"
#include <ctype.h>

extern bool progbar_activated;

void usage() {
    printf("Usage Error.\n");
    printf("Usage: snappy {command} {options} <source> <dest>\n");
    printf("Commands:\n"
           "\t-c      : compress (+ options available)\n"
           "\t-d      : decompress\n"
           "\n"
           "Options:\n"
           "\tIn compression:\n"
           "\t\t-v    : verbose, if selected let the program print all operations. SLOW!\n"
           "\t\t-V1   : use 32k block, a little faster but compress a little less\n"
           "\t\t-V11* : use 64K block\n"
           "\t\t-n*   : use closed hashmap without data structures\n"
           "\t\t        this makes the process really faster but compress little less (10%% ~)\n"
           "\t\t-l    : use a linear list to handle collision of hashmap\n"
           "\t\t        this improves compression (10%% ~), but slows down a lot the process\n"
           "\t\t-p   : print progression bar to show progress (default is activated)"
           "\n"
           "\tIn decompression:\n"
           "\t\t-v    : verbose, if selected let the program print all operations. SLOW!\n"
           "\n"
           "\t* -> default options\n\n");
}

//see usage for parameters
int main(int argc, char *argv[]) {

    if (argc < 4) {
        usage();
        return 1;
    }

    clock_t time;

    //params
    char *command = malloc(3);

    bool verbose = false;
    bool V11 = true;
    bool list = false;
    bool progbar = false;

    char *src_file;
    char *dest_file;

    int options_count = argc - 4;

    //Command setted
    strncpy(command, argv[1], 2);
    command[3] = '\0';

    src_file = argv[argc-2];
    dest_file = argv[argc-1];

    for(int i=2; i<options_count+2; i++){
        if(strncmp(argv[i], "-V11", 4)==0){
            V11=true;
        }else if(strncmp(argv[i], "-V1", 3)==0){
            V11=false;
        }else if(strncmp(argv[i], "-l", 2)==0){
            list=true;
        }else if(strncmp(argv[i], "-n", 2)==0){
            list=false;
        }else if(strncmp(argv[i], "-v", 2)==0){
            verbose=true;
        }else if(strncmp(argv[i], "-p", 3)==0){
            progbar=true;
        }else{
            printf("Option not recognized: %s\n", argv[i]);
        }
    }

    set_options(V11,list,verbose, progbar);

    if (strcmp(command, "-c") == 0) {
        printf("Using options: version: %s, structures: %s\n", V11?"1.1":"1", list?"Linear List":"None");
        printf("Compressing : %s\n", src_file);
        time = clock();
        if (compress(src_file, dest_file)) {
            time = clock() - time;

            printf("\nCompressed to: %s\n", dest_file);
            FILE *fin = fopen(src_file, "r");
            FILE *fout = fopen(dest_file, "r");

            _size_t in = file_size(fin);
            _size_t out = file_size(fout);

            printf("\n\n- %-12s \t%lu B\n- %-12s \t\t%lu B",
                   "Initial Size:",
                   in,
                   "Final Size:",
                   out);
            printf("\n- %-12s \t\t%.3f %%\n","Ratio:", (float) out / (float) in * 100);

            printf("- %-12s \t\t~%5.3f s\n", "Timing:", (float) time / CLOCKS_PER_SEC);
            printf("- %-12s \t\t%5.3f MB/s\n", "Speed:", ((float) in / ((float) time / CLOCKS_PER_SEC)) / 1000000);
            fclose(fin);
            fclose(fout);
            return 0;

        } else {
            perror("Compression Error\n");
            return 1;
        }


    } else if (strcmp(command, "-d") == 0) {
        printf("Decompressing : %s\n", src_file);
        time = clock();
        if (decompress(src_file, dest_file)) {
            FILE *fin = fopen(src_file, "r");
            FILE *fout = fopen(dest_file, "r");

            _size_t in = file_size(fin);
            _size_t out = file_size(fout);

            printf("\n\n- %-12s \t%lu B\n- %-12s \t\t%lu B",
                   "Initial Size:",
                   in,
                   "Final Size:",
                   out);
            printf("\n- %-12s \t\t%.3f %%\n","Ratio:", (float) out / (float) in * 100);

            printf("- %-12s \t\t~%5.3f s\n", "Timing:", (float) time / CLOCKS_PER_SEC);
            printf("- %-12s \t\t%5.3f MB/s\n", "Speed:", ((float) out / ((float) time / CLOCKS_PER_SEC)) / 1000000);
            fclose(fin);
            fclose(fout);
            return 0;
        } else {
            perror("Decompression Error\n");
            return 1;
        }
    }else{
        printf("Command not recognized: %s\n", command);
        usage();
    }

}