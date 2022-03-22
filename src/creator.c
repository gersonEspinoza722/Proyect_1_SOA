#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include "../include/shared_mem.h"
shared_memory *sm_ptr;
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
      // ftok to generate unique key 
    key_t key = ftok(".",65); 

    key_t key1 = ftok(".",'B');
    key_t key2 = ftok(".",'C');
    key_t key3 = ftok(".",'D');
    key_t key4 = ftok(".",'E');
    key_t key5 = ftok(".",'F');
    key_t key6 = ftok(".",'G');
    key_t key7 = ftok(".",'H');
    key_t key8 = ftok(".",'I');
    key_t key9 = ftok(".",'J');
  
    // shmget returns an identifier in shmid 
    int shmid = shmget(key,1024,0666|IPC_CREAT); 

    // attach to shared memory
    sm_ptr = (shared_memory*) shmat(shmid, 0,0); 

    // store the ids of the circular buffer and the message array, they will be used
    // on the producer and consumer to attach them to the their respective space in the shared memory
    sm_ptr->cb_shmid = shmget(key1, sizeof(shared_buffer), IPC_CREAT|0666);
    sm_ptr->m_shmid = shmget(key2, sizeof(message), IPC_CREAT|0666);
    //sm_ptr->semid = semget(key3, 3, 0666 | IPC_CREAT);
    
    // attach this process to the shared memory. This will also initialize their memory spaces 
    sm_ptr->buffer = (shared_buffer*)shmat(sm_ptr->cb_shmid, NULL, 0);
    (*sm_ptr->buffer).messages = (message*)shmat(sm_ptr->m_shmid, NULL, 0);
}
