#ifndef TIMER
#define TIMER

#include <stdio.h>
#include <time.h>

typedef struct{
    //variables to take snapshots of time instants
    //-Execution time (up time)
    //-Time spent in delay of exponential distribution
    //-Time spent while waiting for shared memory
    long int start_execution_time, end_execution_time;
    long int start_delay_time, end_delay_time;
    long int start_waiting_time, end_waiting_time;

    //cronometers
    int execution_time;
    int time_spent_in_exp_dist_delay;
    int time_spent_waiting_shared_memory;
}crono;



char* get_current_date();
void init_cronometer(int *crono);
void cronometer_start(long int *start);
void cronometer_end(long int *end);
void save_time_spent(long int *start, long int  *end, int *cpu_time_used);
//double get_total_time_spent();


#endif
