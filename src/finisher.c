#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "../include/timer.h"
#include "../include/shared_mem.h"
#include "../include/buffer.h"
#include <math.h>

shared_memory *sm_ptr;
message *msg;
crono Cronometers;
int total_consumers = 0;
int total_producers = 0;
int exponential_dist_setup(){
    srand(time(NULL));   // Initialization, should only be called once.
    return 0;
}

int exponential_dist(int mean){
    double exponential_dist_double=0;
    int exponential_dist = 0;
    int r = rand();      // Returns a pseudo-random integer between 0 and RAND_MAX.
    #ifdef DEBUG
    printf("This is r: %d \n", r);
    #endif
    double linear_dist_prob = (double)r / RAND_MAX;

    #ifdef DEBUG
    printf("This is linear_dist_prob: %f \n", linear_dist_prob);
    #endif
    
    exponential_dist_double=-(double)mean*log(1-linear_dist_prob);
    #ifdef DEBUG
    printf("This is exponential dist double: %f \n", exponential_dist_double);
    #endif
    exponential_dist = (int)exponential_dist_double;
    #ifdef DEBUG
    printf("Exponential_dist: %d \n", exponential_dist);
    #endif
    return exponential_dist;
}
int main(int argc, char **argv)
{
    int user_input;
    char usage_msj[] = "Usage: ./finalizer_main sharedMemoryId(int)\n %s";

    if(argc != 2){
        fprintf(stderr, usage_msj, "Parameter count!\n");
        return EXIT_FAILURE;
    } 
    //get the id from argv
    int shmid = atoi(&(*argv[1]));

    if (shmid < 1)
    {
        fprintf(stderr, usage_msj, "All values must be a positive number greater than zero!\n");
        return EXIT_FAILURE;
    }
    printf("finalizer program\n");
    init_cronometer(&Cronometers.execution_time);
    init_cronometer(&Cronometers.time_spent_waiting_shared_memory);
    cronometer_start(&Cronometers.start_execution_time);

    // shmat to attach to shared memory
    sm_ptr = (shared_memory *)shmat(shmid, 0, 0);

    // semaphores configuration
    struct sembuf sb = {0, -1, 0};
    struct sembuf sb1 = {1, 1, 0};
    struct sembuf sb2 = {2, -1, 0};

    int total_consumers = sm_ptr->consumers_count;
    int total_producers = sm_ptr->producers_count;
    int total_messages_finalizer = 0;

    printf("Please input something to kill producers: \n");
    scanf("%d \n", &user_input);
    printf("You entered: %d\n", user_input);

    (sm_ptr->suspend_producers) = true;

    printf("Producers should be finalizing by now \n");

    int pid = getpid();

    while (sm_ptr->consumers_count > 0) // this condition should be changed so the loop goes until the producer is said to no longer produce
    {

        //producer is going to wait a exp_delay that is a random number calculated from exponential distribution
        usleep(100000);

        //timestamp from the instant when request to shared memory starts
        cronometer_start(&Cronometers.start_waiting_time);
        semop(sm_ptr->semid, &sb2, 1); // prevents overflow
        semop(sm_ptr->semid, &sb, 1);  // controls buffer access
        //timestamp from the instant when request to shared memory starts
        cronometer_end(&Cronometers.end_waiting_time);
        save_time_spent(&Cronometers.start_waiting_time, &Cronometers.end_waiting_time, &Cronometers.time_spent_waiting_shared_memory);

        // attach buffer and messages structures to the shared memory
        sm_ptr->buffer = (shared_buffer *)shmat(sm_ptr->cb_shmid, NULL, 0);
        (*sm_ptr->buffer).messages = (message *)shmat(sm_ptr->m_shmid, NULL, 0);

        sm_ptr->total_messages++;
        // append message to the buffer
        CB_push(sm_ptr->buffer, pid, 6, sm_ptr->consumers_count, sm_ptr->producers_count, sm_ptr->total_messages);
        total_messages_finalizer++;

        sb.sem_op = 1;
        semop(sm_ptr->semid, &sb, 1);
        semop(sm_ptr->semid, &sb1, 1); // prevents underflow
    }
    printf("===============================================================\n");
    printf("=== Report of Finalizer with id: %5d                      ===\n", pid);
    printf("=== Number of sent messages    : %5d                      ===\n", total_messages_finalizer);
    printf("=== Total number of sent messages    : %5d                ===\n", sm_ptr->total_messages);
    printf("=== Total execution time is: %5d (s)                      ===\n", Cronometers.execution_time);
    printf("=== Total of finalized consumers: %5d                     ===\n", total_consumers);
    printf("=== total of finalized producers: %5d                     ===\n", total_producers);
    printf("===============================================================\n");

        // destroy the shared memory
        shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
