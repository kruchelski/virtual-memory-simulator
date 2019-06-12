# VIRTUAL MEMORY SIMULATOR
---
## Activity for the Operating System discipline, of the third semester of the UFPR TADS course.

### Credits
Developed in colaboration with Andre Klingenfus Antunes [Github page](https://github.com/andrekantunes)

### Description
This little application explores three algorithms for memory allocation:
1. Least Recently Used - LRU
2. Least Recently Used Reference Bit
3. Second Chance

The program takes an input in the form of a txt file with the instructions to the "CPU". The instructions can be the creation of a process, read and write at specif memory locations.
- Example of a file:
> P1 C 500 KB
> P2 C 100 KB
> P1 R 0x010
> P2 W 0xfff

In the above example, the first line indicates the creation (C) of process P1 and it's size: 500 KB. The second line indicates the creation (C) of process P2 and it's size: 100 KB. The third line indicates the READ instruction for process P1 in the memory location of 0x010. The last line indicates the WRITE instruction for the process P2 in the memory location of 0xfff.
The READ and WRITE instructions are treated equally for the purposes of this application. The only difference is the CREATE instruction.

**IMPORTANT: The .txt file must be structured exactly like the two provided in the repository**

### Instructions:

After compiling, in order to execute from the command line interface, it's important to pass the proper arguments:

- -p followed by the frame size in Bytes
- -m followed by the size of the main memory
*OBS: the size of the main memory must be a multiple of the frame size* 

In case of a problem with the arguments, the program will use it's default values:
- Frame size: 1000B
- Main memory size: 4000B

The program will receive, as input file, a .txt with the name "process.txt" which have some instructions to be executed.
The output will show how pages are being allocated in the frames of the main memory.

