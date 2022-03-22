#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
//#include "circular_buffer.h"
//#include "semaphore_union.h"

#define MESSAGE_MAX_SIZE (size_t)4096

typedef struct
{
    int total_messages;
    bool suspend_producers;
    int producers_count;
    int consumers_count;

    int cb_shmid;
    int m_shmid;
    int semid;

    //union semun sem1_arg;
    //union semun sem2_arg;
    //union semun sem3_arg;
    
    //circular_buffer *buffer;
} shared_memory;

int shm_create();

#endif
