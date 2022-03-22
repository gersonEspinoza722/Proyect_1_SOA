#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include "../include/shared_mem.h"
int main(int argc, char **argv)
{ 
    char usage_msj[] = "Usage: ./creator_main bufferSize(int)\n %s";


    if(argc != 2){
        fprintf(stderr, usage_msj, "Parameter count!\n");
        return EXIT_FAILURE;
    }

    //get the buffersize from argv
    int buffer_size = atoi(&(*argv[1]));

    if(buffer_size < 1){
        fprintf(stderr, usage_msj, "Buffer size must be a positive number greater than zero!\n");
        return EXIT_FAILURE;
    }
}
