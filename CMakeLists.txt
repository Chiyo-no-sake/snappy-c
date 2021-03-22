cmake_minimum_required(VERSION 3.15)
project(Snappy C)

set(CMAKE_C_STANDARD 99)

add_executable(Snappy
        # README
        # put here every test file or every file with a main function in it
        # also remember to leave only 1 line of these uncommented to let the compiler know what to run.
        # |     |
        # V     V
        #src/test_bindata.c
        #src/test_hashmap.c
        #src/test_avl.c
        src/snappy.c

        #add here, if not done autmatically by IDE, all files that are part of the projet
        src/snappy_algo.c
        src/snappy_algo.h

        src/utilityfuncs.c
        src/utilityfuncs.h

        src/list.c
        src/list.h

        src/hashtable.c
        src/hashtable.h

        )