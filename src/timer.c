#include "../include/timer.h"

//#define DEBUG 1

char* get_current_date()
{
    char* result;

    time_t t =time(NULL);
    struct tm *tm = localtime(&t);
    result = asctime(tm);
    
    return result;
}

void init_cronometer(int *crono){
    *crono=0;
}

void cronometer_start(long int *start){
    *start=time(NULL);
    #ifdef DEBUG
        printf("The value for start is: %ld \n", *start);
    #endif
}

void cronometer_end(long int *end){
    *end=time(NULL);
}

void save_time_spent(long int *start,long int *end, int *cpu_time_used){
    int lap_time = *end - *start;
    *cpu_time_used = *cpu_time_used + lap_time;
}

// double get_total_time_spent(){
//     return cpu_time_used;
// }
