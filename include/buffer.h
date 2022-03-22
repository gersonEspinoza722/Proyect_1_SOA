#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//#include "data_structures/message.h"
#include "message.h"

typedef struct
{
    int size;
    int start;
    int count;

    message *messages;
} shared_buffer;

void CB_init(shared_buffer *cb, int size);
void CB_push(shared_buffer *cb, int pid, int k,  int total_consumer, int total_producer, int message_id);
message CB_pop(shared_buffer *cb, int pid,  int total_consumer, int total_producer);
bool CB_full(shared_buffer *cb);
bool CB_empty(shared_buffer *cb);

#endif
