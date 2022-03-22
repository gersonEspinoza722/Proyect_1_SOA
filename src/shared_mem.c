#include "../include/shared_mem.h"

int shm_create()
{
    // ftok to generate unique key
    key_t key = ftok("shmfile", 65);

    int shm_id = 0;

    if (0 > (shm_id = shmget(
                 key, sizeof(shared_buffer) + MESSAGE_MAX_SIZE + sizeof(size_t), IPC_CREAT | IPC_EXCL | 0666)))
    {
        /* An error occurred, add desired error handling. */
    }

    return shm_id;
}
