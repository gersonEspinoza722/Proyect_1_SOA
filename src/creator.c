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
    
    // suspend producers initialize
    sm_ptr->suspend_producers=false;

    // initialize counters
    sm_ptr->total_messages = 0;
    sm_ptr->consumers_count = 0;
    sm_ptr->producers_count = 0;

    // initialize semaphores. Will use a semaphore set of 3 as follow:
    //      1. semaphore 0: controls buffer access
    //      2. semaphore 1: prevents underflow
    //      3. semaphore 2: prevents overflow

    // semaphore 0 initialization
    sm_ptr->sem1_arg.array = (unsigned short*)shmat(shmget(key4, sizeof(unsigned short), IPC_CREAT|0666),0,0);
    sm_ptr->sem1_arg.buf = (struct semid_ds*)shmat(shmget(key5, sizeof(struct semid_ds), IPC_CREAT|0666),0,0);
    sm_ptr->sem1_arg.val = 1;
    semctl(sm_ptr->semid, 0, SETVAL, sm_ptr->sem1_arg);

    // semaphore 1 initialization
    sm_ptr->sem2_arg.array = (unsigned short*)shmat(shmget(key6, sizeof(unsigned short), IPC_CREAT|0666),0,0);
    sm_ptr->sem2_arg.buf = (struct semid_ds*)shmat(shmget(key7, sizeof(struct semid_ds), IPC_CREAT|0666),0,0);
    sm_ptr->sem2_arg.val = 0;
    semctl(sm_ptr->semid, 1, SETVAL, sm_ptr->sem2_arg);

    // semaphore 2 initialization
    sm_ptr->sem3_arg.array = (unsigned short*)shmat(shmget(key8, sizeof(unsigned short), IPC_CREAT|0666),0,0);
    sm_ptr->sem3_arg.buf = (struct semid_ds*)shmat(shmget(key9, sizeof(struct semid_ds), IPC_CREAT|0666),0,0);
    sm_ptr->sem3_arg.val = buffer_size;
    semctl(sm_ptr->semid, 2, SETVAL, sm_ptr->sem3_arg);    
    
    // circular buffer initialization
    CB_init(sm_ptr->buffer, buffer_size);

    printf("shared memory created! your id is %d\n", shmid);

    return 0;
}
