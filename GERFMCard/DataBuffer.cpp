#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DataBuffer.h"

void dataBufferInit(TY_BUFFER_FIFO *dataBuffer, unsigned int block_size)
{
	//pthread_rwlock_init(&dataBuffer->data_lock, NULL);
	int i = 0;
	dataBuffer->write_index = 0;
	dataBuffer->read_index = 0;
	dataBuffer->block_size = block_size + 2;
	char *pTemp;
	for (i = 0; i < MAX_BLOCK_NUM; i++)
	{
		dataBuffer->data_block[i] = (char*) malloc(
			dataBuffer-> block_size);
	}
}
void dataBufferClear(TY_BUFFER_FIFO *dataBuffer)
{
	char *pTemp;
	int i;
	for (i = 0; i < MAX_BLOCK_NUM; i++)
	{
		if (dataBuffer->data_block[i] != NULL)
		{
			pTemp = dataBuffer->data_block[i];
			if (pTemp != NULL)
			{
				free(pTemp);
				dataBuffer->data_block[i] = NULL;
			}
		}
	}
	//pthread_rwlock_destroy(&dataBuffer->data_lock);
}

//read_data
int dataBufferRead(TY_BUFFER_FIFO *dataBuffer, char *recv_buf,
		unsigned short *data_len)
{
	//pthread_rwlock_wrlock(&dataBuffer->data_lock);

	if (dataBuffer->data_block[dataBuffer->read_index] == NULL)/* added by ping.song */
		return E_BUFFER_NOINIT;

	if (dataBuffer->read_index == dataBuffer->write_index)
	{
		//pthread_rwlock_unlock(&dataBuffer->data_lock);
		return E_BUFFER_EMPTY;
	}

//	printf("dataBuffer_read_data----enter \n");

	//compute the data len
	*data_len
			= (unsigned char) dataBuffer->data_block[dataBuffer->read_index][0]
					* 256
					+ (unsigned char) dataBuffer->data_block[dataBuffer->read_index][1];
	memcpy(recv_buf, dataBuffer->data_block[dataBuffer->read_index] + 2,
			*data_len);

	dataBuffer->read_index++;
	dataBuffer->read_index %= MAX_BLOCK_NUM;

	//pthread_rwlock_unlock(&dataBuffer->data_lock);
//	printf("dataBuffer_read_data---exit\n");
	return DATA_BUFFER_SUCCESS;
}
int dataBufferWrite(TY_BUFFER_FIFO *dataBuffer, char *write_data,
		unsigned short data_len)
{
	//printf(" write_index:%d  read_index:%d \n", dataBuffer->write_index, dataBuffer->read_index);
	//pthread_rwlock_wrlock(&dataBuffer->data_lock);
	if (dataBuffer->data_block[dataBuffer->write_index] == NULL)/* added by ping.song */
		return E_BUFFER_NOINIT;

	dataBuffer->data_block[dataBuffer->write_index][0] = data_len / 256;
	dataBuffer->data_block[dataBuffer->write_index][1] = data_len % 256;
	memcpy(dataBuffer->data_block[dataBuffer->write_index] + 2, write_data,
			data_len);
	dataBuffer->write_index++;
	dataBuffer->write_index %= MAX_BLOCK_NUM;
	if (dataBuffer->write_index + 1 == dataBuffer->read_index)
	{
		//pthread_rwlock_unlock(&dataBuffer->data_lock);
		return E_BUFFER_FLOW;
	}
	//pthread_rwlock_unlock(&dataBuffer->data_lock);
	return DATA_BUFFER_SUCCESS;
}
void dataBufferReset(TY_BUFFER_FIFO *dataBuffer)
{
	dataBuffer->read_index = 0;
	dataBuffer->write_index = 0;
}
