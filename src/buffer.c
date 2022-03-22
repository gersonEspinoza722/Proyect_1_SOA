#include <string.h>

#include "../utilities/time_util.h"
#include "buffer.h"
#include "message.h"

message *msg;


void CB_init(shared_buffer *cb, int size)
{
    cb->size = size;
    cb->start = 0;
    cb->count = 0;
}

void CB_push(shared_buffer *cb, int pid, int k, int total_consumer, int total_producer, int message_id)
{
    int index;
    if (CB_full(cb))
    {
        printf("Buffer overflow\n");
    }
    else
    {
        index = cb->start + cb->count++;
        if (index >= cb->size)
        {
            index = 0;
        }
        cb->messages[index].id = message_id;
        cb->messages[index].key = k;
        cb->messages[index].pid = pid;
        strcpy(cb->messages[index].date, get_current_date());

        printf("Producer PID: %d, has pushed a message with id: %d to index: %d at %s, key: %d, total producers: %d, total consumers: %d\n",
        pid, message_id, index, cb->messages[index].date, k,total_producer, total_consumer);
    }
}

message CB_pop(shared_buffer *cb, int pid, int total_consumer, int total_producers)
{
    message element;
    int index=0;
    if (CB_empty(cb))
    {
        printf("Buffer underflow\n");
        return element;
    }
    else
    {
        element = cb->messages[cb->start];
        index=cb->start;
        cb->start++;
        cb->count--;
        if (cb->start == cb->size)
        {
            cb->start = 0;
        }

        printf("Consumer PID: %d, has pulled a message with id: %d from index: %d, key: %d, Total consumer: %d, Total producers: %d, at %s \n",
        pid, cb->messages[index].id , index, cb->messages[index].key, total_consumer, total_producers, cb->messages[index].date );

        return element;
    }
}

bool CB_empty(shared_buffer *cb)
{
    if (cb->count == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CB_full(shared_buffer *cb)
{
    if (cb->count == cb->size)
    {
        return true;
    }
    else
    {
        return false;
    }
}
