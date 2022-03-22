#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "../include/timer.h"
//#include "../utilities/exp_dist.h"
#include "../include/shared_mem.h"

shared_memory *sm_ptr;
message *msg;
crono Cronometers;

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
    int number_of_produced_messages=0;
    char usage_msj[] = "Usage: ./producer_main sharedMemoryId(int) productionTime(int)\n %s";

    //Setting random for exponential distribution
    exponential_dist_setup();
    
    if(argc != 3){
        fprintf(stderr, usage_msj, "Parameter count!\n");
        return EXIT_FAILURE;
    } 

    // get the id from argv
    int shmid = atoi(&(*argv[1]));

    // production time (we ask the user for seconds but need useconds)
    // we need to integrate Marco's computation for exponential distribution
    // the param index for the time will eventually be different
    long pt = atoi(&(*argv[2]));

    if(shmid < 1 || pt < 1){
        fprintf(stderr, usage_msj, "All values must be a positive number greater than zero!\n");
        return EXIT_FAILURE;
    }

    pt = pt * 1000000;

    init_cronometer(&Cronometers.execution_time);
    init_cronometer(&Cronometers.time_spent_in_exp_dist_delay);
    init_cronometer(&Cronometers.time_spent_waiting_shared_memory);
    cronometer_start(&Cronometers.start_execution_time);

    //printf("The value for start execution is : %ld", Cronometers.start_execution_time);
    //printf("Im getting to crono start\n");
    

    // semaphores configuration
    struct sembuf sb  = {0, -1, 0};
    struct sembuf sb1 = {1, 1, 0};
    struct sembuf sb2 = {2, -1, 0};

    // shmat to attach to shared memory
    sm_ptr = (shared_memory *)shmat(shmid, 0, 0);

    sm_ptr->producers_count++;
    int pid = getpid();

    
    while (1) // this condition should be changed so the loop goes until the producer is said to no longer produce
    {
        //check if finalizer requested end of life
        //printf("this is the value of suspend_producers: %s", sm_ptr->suspend_producers? "true": "false");
        if(sm_ptr->suspend_producers==true){
            sm_ptr->producers_count--;
            break;
        }
        //calculating random delay with exponential distribution with mean of "pt"
        int exp_delay=exponential_dist(pt); 

        //timestamp from the instant when delay starts
        cronometer_start(&Cronometers.start_delay_time);
        //printf("The value for start delay is : %ld", Cronometers.start_delay_time);

        //producer is going to wait a exp_delay that is a random number calculated from exponential distribution
        usleep(exp_delay);

        //timestamp from the instant when delay ends
        cronometer_end(&Cronometers.end_delay_time);
        //printf("The value for end delay is : %ld", Cronometers.end_delay_time);

        //save the time spent in last delay
        save_time_spent(&Cronometers.start_delay_time, &Cronometers.end_delay_time, &Cronometers.time_spent_in_exp_dist_delay);

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
        CB_push(sm_ptr->buffer, pid, rand()%5,sm_ptr->consumers_count,sm_ptr->producers_count, sm_ptr->total_messages);
        //increment produced messages
        number_of_produced_messages++;



        sb.sem_op = 1;
        semop(sm_ptr->semid, &sb, 1);
        semop(sm_ptr->semid, &sb1, 1); // prevents underflow
    }

    //producers need to print some statistics for instance:
    //-Id
    //-Number of produced messages
    //-Execution time (up time)
    //-Time spent in delay of exponential distribution
    //-Time spent while waiting for shared memory
    //-Good Bye message
    cronometer_end(&Cronometers.end_execution_time);
    //printf("The value for end execution is : %ld", Cronometers.end_execution_time);
    save_time_spent(&Cronometers.start_execution_time, &Cronometers.end_execution_time, &Cronometers.execution_time);

    printf("===============================================================\n");
    printf("=== Report of producer with id: %5d                        ===\n",pid);
    printf("=== Number of messages produced: %5d                       ===\n",number_of_produced_messages);
    printf("=== Total execution time is: %5d (s)                       ===\n",Cronometers.execution_time);
    printf("=== Total time spent in delay is: %5d (s)                  ===\n",Cronometers.time_spent_in_exp_dist_delay);
    printf("=== Total time spent waiting for shared mem is: %5d (s)    ===\n",Cronometers.time_spent_waiting_shared_memory);
    printf("===============================================================\n");

    //detach from shared memory
    shmdt(sm_ptr);

    return 0;
}
