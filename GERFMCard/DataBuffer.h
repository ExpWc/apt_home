#ifndef _DATA_BUFFER_H
#define _DATA_BUFFER_H

// #include <semaphore.h>
// #include <pthread.h>

#define E_BUFFER_EMPTY 1
#define E_BUFFER_FLOW  2
#define DATA_BUFFER_SUCCESS      0
#define E_BUFFER_NOINIT 3	/* added by ping.song */
#define MAX_BLOCK_NUM 4096

typedef struct bufferfifo
{
        unsigned int write_index;
        unsigned int read_index;
        unsigned int count;
        unsigned int block_size;
        char *data_block[MAX_BLOCK_NUM];
       // pthread_rwlock_t data_lock;
}TY_BUFFER_FIFO;

//extern TY_BUFFER_FIFO rfm_dataBuffer;

void
dataBufferInit(TY_BUFFER_FIFO *dataBuffer, unsigned int block_size);
void
dataBufferClear(TY_BUFFER_FIFO *dataBuffer);
//read_data
int
dataBufferRead(
        TY_BUFFER_FIFO *dataBuffer,
        char *recv_buf,
        unsigned short *data_len);
int
dataBufferWrite(
        TY_BUFFER_FIFO *dataBuffer,
        char *write_data,
        unsigned short data_len);
void
dataBufferReset(TY_BUFFER_FIFO *dataBuffer);

#endif
