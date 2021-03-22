# Snappy-C
An alternative Google's Snappy Algorithm for compressing files much stronger.
This implementation is compatible with Microcontrollers sicne it's written in basic C instead of C++.

There are 2 ways of using this algotithm: the first is to download from the reelase page,
the second is to build from source.

## Building sources
To build sources you just have to clone the repo in a folder of your choice, after that, enter the directory and execute:
```
cmake .
make
```

to build the program.

## Usage
Using snappy-c algorithm is simple: just run `./Snappy -c <source> <dest>` with `<source>` being the file to covert and `<dest>` being the output file name.
You can specify other options for the compression stage only:
```
        -v    : verbose, if selected let the program print all operations. SLOW!
		-V1   : use 32k block, a little faster but compress a little less
		-V11* : use 64K block
		-n*   : use closed hashmap without data structures
		        this makes the process really faster but compress little less (10% ~)
		-l    : use a linear list to handle collision of hashmap
		        this improves compression (10% ~), but slows down a lot the process
		-p   : print progression bar to show progress (default is activated)
```

and in decompression: 
```-v    : verbose, if selected let the program print all operations. SLOW!```

