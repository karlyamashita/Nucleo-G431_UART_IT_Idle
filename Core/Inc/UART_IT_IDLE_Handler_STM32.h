/*
 * UART_IT_IDLE_Handler.h
 *
 *  Created on: Jan 3, 2023
 *      Author: karl.yamashita
 */

#ifndef INC_UART_IT_IDLE_HANDLER_H_
#define INC_UART_IT_IDLE_HANDLER_H_

// USER DEFINES User can adjust these defines to fit their project requirements
#define UART_IT_IDLE_DATA_SIZE 128
#define UART_IT_IDLE_QUEUE_SIZE 8
// END USER DEFINES
// **************************************************
// ********* Do not modify code below here **********
// **************************************************
typedef struct
{
	uint32_t size; // move to top to byte align
	uint8_t data[UART_IT_IDLE_DATA_SIZE];
}UART_IT_IDLE_Data_t; // this is used in queue structure

typedef struct
{
	UART_HandleTypeDef *huart;
	struct
	{
		UART_IT_IDLE_Data_t queue[UART_IT_IDLE_QUEUE_SIZE];
		UART_IT_IDLE_Data_t *msgToParse;
		RING_BUFF_STRUCT ptr;
		uint32_t queueSize;
		uint32_t IT_dataSize; // the maximum data length you are expecting.
		HAL_StatusTypeDef hal_status;
	}rx;
	struct
	{
		UART_IT_IDLE_Data_t queue[UART_IT_IDLE_QUEUE_SIZE];
		RING_BUFF_STRUCT ptr;
		uint32_t queueSize;
	}tx;
}UART_IT_IDLE_Queue_t;


void UART_IT_IDLE_Init(UART_IT_IDLE_Queue_t *msg, UART_HandleTypeDef *huart);
void UART_IT_IDLE_EnableRxInterrupt(UART_IT_IDLE_Queue_t *msg);
void UART_IT_IDLE_CheckRxInterruptErrorFlag(UART_IT_IDLE_Queue_t *msg);
int UART_IT_IDLE_MsgRdy(UART_IT_IDLE_Queue_t *msg);
void UART_IT_IDLE_NotifyUser(UART_IT_IDLE_Queue_t *msg, char *str, uint32_t size, bool lineFeed);

void UART_IT_IDLE_TX_AddMessageToBuffer(UART_IT_IDLE_Queue_t *msg, uint8_t *data, uint32_t size);
void UART_IT_IDLE_SendMessage(UART_IT_IDLE_Queue_t * msg);


#endif /* INC_UART_IT_IDLE_HANDLER_H_ */
