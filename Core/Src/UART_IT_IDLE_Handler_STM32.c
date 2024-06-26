/*
 * UART_IT_IDLE_Handler.c
 *
 *  Created on: Jan 3, 2023
 *      Author: karl.yamashita
 *
 *      FOR STM32 with HAL_UARTEx_ReceiveToIdle_IT and HAL_UARTEx_RxEventCallback
 *
 */

#include "main.h"


/*
 * Description: Enable rx interrupt. Save the HAL status which we'll check later in main while loop;
 *
 */
void UART_IT_IDLE_EnableRxInterrupt(UART_IT_IDLE_Queue_t *msg)
{
	msg->rx.hal_status = HAL_UARTEx_ReceiveToIdle_IT(msg->huart, msg->rx.queue[msg->rx.ptr.index_IN].data, msg->rx.IT_dataSize);
}

/*
 * Description: Call from polling routine. If HAL status != HAL_OK, then we need to try to enable Rx interrupt again.
 *
 */
void UART_IT_IDLE_CheckRxInterruptErrorFlag(UART_IT_IDLE_Queue_t *msg)
{
	if(msg->rx.hal_status != HAL_OK)
	{
		msg->rx.hal_status = HAL_OK;
		UART_IT_IDLE_EnableRxInterrupt(msg);
	}
}


/*
 * Description: Return 0 if no new message, 1 if there is message in msgOut
 */
int UART_IT_IDLE_MsgRdy(UART_IT_IDLE_Queue_t *msg)
{
	if(msg->rx.ptr.cnt_Handle)
	{
		msg->rx.msgToParse = &msg->rx.queue[msg->rx.ptr.index_OUT];
		RingBuff_Ptr_Output(&msg->rx.ptr, msg->rx.queueSize);
		return 1;
	}

	return 0;
}

/*
* Description: Add message to TX buffer
*/
void UART_IT_IDLE_TX_AddMessageToBuffer(UART_IT_IDLE_Queue_t *msg, uint8_t *data, uint32_t size)
{
	UART_IT_IDLE_Data_t *ptr = &msg->tx.queue[msg->tx.ptr.index_IN];

	memcpy(ptr->data, data, size);
	ptr->size = size;

    RingBuff_Ptr_Input(&msg->tx.ptr, msg->tx.queueSize);
}

/*
 * Description: This will be called from UART_IT_IDLE_NotifyUser or from HAL_UART_TxCpltCallback.
 * 				The queue pointer is only incremented if HAL status returns HAL_OK.
 * 				Otherwise the message was not sent because HAL is still busy sending.
 *
 */
void UART_IT_IDLE_SendMessage(UART_IT_IDLE_Queue_t * msg)
{
	if(msg->tx.ptr.cnt_Handle)
	{
		if(HAL_UART_Transmit_IT(msg->huart, msg->tx.queue[msg->tx.ptr.index_OUT].data, msg->tx.queue[msg->tx.ptr.index_OUT].size) == HAL_OK)
		{
			RingBuff_Ptr_Output(&msg->tx.ptr, msg->tx.queueSize);
		}
	}
}

/*
* Description: Add string to TX queue. Then  UART_IT_IDLE_SendMessage is called to send the message.
*/
void UART_IT_IDLE_NotifyUser(UART_IT_IDLE_Queue_t *msg, char *str, uint32_t size, bool lineFeed)
{
	uint8_t strMsg[UART_IT_IDLE_DATA_SIZE] = {0};

    strcpy((char*)strMsg, str);
    
    if(lineFeed == true)
    {
    	strcat((char*)strMsg, "\r\n");
    	size += 2; // add 2 due to CR and LF
    }

    UART_IT_IDLE_TX_AddMessageToBuffer(msg, strMsg, size); // add message to queue

    UART_IT_IDLE_SendMessage(msg); // Send message using HAL_UART_Transmit_IT
}


/*
 - Below is an example of checking for a new message and have msgToParse as a pointer to the queue.
 - It is totally up to the user how to send messages to the STM32 and how to parse the messages.
 - User would call UART_CheckForNewMessage(&uartDMA_RXMsg) from a polling routine

void UART_CheckForNewMessage(UART_IT_IDLE_QueueStruct *msg)
{
	if(UART_IT_IDLE_MsgRdy(msg))
	{
		// user can parse msg variable.
		if(strncmp(msg->msgToParse->data, "get version", strlen("get version")) == 0)
		{
			// call function to return version number
		}
		else if(strncmp(msg->msgToParse->data, "get status", strlen("get status")) == 0)
		{
			// call function to get status information
		}
	}
}

// Here are callbacks that should be placed in your polling routine or interrupt routine.
 * This is for uart1. If you have more UART instances then test for those.

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart == uart1.huart)
	{
		uart1.rx.queue[uart1.rx.ptr.index_IN].size = Size;
		RingBuff_Ptr_Input(&uart1.rx.ptr, uart1.rx.queueSize);
		UART_IT_IDLE_EnableRxInterrupt(&uart1);
	}
	else if(huart == uart2.huart)
	{
		uart2.rx.queue[uart2.rx.ptr.index_IN].size = Size;
		RingBuff_Ptr_Input(&uart2.rx.ptr, uart2.rx.queueSize);
		UART_IT_IDLE_EnableRxInterrupt(&uart2);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == uart1.huart)
	{
		UART_IT_IDLE_SendMessage(&uart1);
	}
	else if(huart == uart2.huart)
	{
		UART_IT_IDLE_SendMessage(&uart2);
	}
}

// Be sure to initialize UART instance in polling routine
 *
UART_IT_IDLE_QueueStruct uart1 =
{
	.huart = &hlpuart1,
	.rx.queueSize = UART_IT_IDLE_QUEUE_SIZE,
	.tx.queueSize = UART_IT_IDLE_QUEUE_SIZE
};


 */


