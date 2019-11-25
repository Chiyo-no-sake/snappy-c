cmake_minimum_required(VERSION 3.15)
project(Snappy C)

set(CMAKE_C_STANDARD 99)

add_executable(Snappy src/test.c src/snappylib.c)