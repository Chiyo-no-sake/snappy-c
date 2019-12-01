cmake_minimum_required(VERSION 3.15)
project(Snappy C)

set(CMAKE_C_STANDARD 99)

add_executable(Snappy
        #src/test_bindata.c
        src/test_hashmap.c
        #src/test_avl.c

        src/snappy_algo.c
        src/snappy_algo.h

        src/utilityfuncs.c
        src/utilityfuncs.h

        src/hashtable.c
        src/hashtable.h

        src/avl_tree.c
        src/avl_tree.h)