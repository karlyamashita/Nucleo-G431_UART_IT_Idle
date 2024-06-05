/*
 * PollingRoutine.c
 *
 *  Created on: Oct 24, 2023
 *      Author: karl.yamashita
 *
 *
 *      Template for projects.
 *
 */


#include "main.h"


extern UART_HandleTypeDef huart2;
extern TimerCallbackStruct timerCallback;

char version[] = "1.1.0";

UART_IT_IDLE_Queue_t uart2 =
{
	.huart = &huart2,
	.rx.queueSize = UART_IT_IDLE_QUEUE_SIZE,
	.tx.queueSize = UART_IT_IDLE_QUEUE_SIZE,
	.rx.IT_dataSize = 36
};

void PollingInit(void)
{
	UART_IT_IDLE_EnableRxInterrupt(&uart2); // enable Rx interrupt

	UART_IT_IDLE_NotifyUser(&uart2, "STM32 ready!", strlen("STM32 ready!"), true);

	TimerCallbackRegisterOnly(&timerCallback, Blink_LED);
	TimerCallbackTimerStart(&timerCallback, Blink_LED, 500, TIMER_REPEAT); // blink the LED
}

void PollingRoutine(void)
{
	TimerCallbackCheck(&timerCallback);

	UART2_Parse(&uart2);

	UART_IT_IDLE_CheckRxInterruptErrorFlag(&uart2);
}

void UART2_Parse(UART_IT_IDLE_Queue_t *msg)
{
	int status = 0;
	char *ptr;
	char retStr[64] = "ok";
	char msg_copy[64] = {0};

	if(UART_IT_IDLE_MsgRdy(msg))
	{
		ptr = (char*)msg->rx.msgToParse->data;
		memcpy(&msg_copy, ptr, strlen((char*)ptr) - 2); // remove CR/LF
		RemoveSpaces((char*)ptr);
		ToLower((char*)ptr);

		if(strncmp((char*)ptr, "helloworld", strlen("helloworld")) == 0)
		{
			status = HelloWorld(); // status = NO_ACK
		}
		else if(strncmp((char*)ptr, "cmd5:", strlen("cmd5:")) == 0)
		{
			status = ParseCMD5((char*)ptr); // status =  NO_ERROR
		}
		else if(strncmp((char*)ptr, "getversion", strlen("getversion")) == 0)
		{
			status = GetVersion(retStr); // status = NO_ERROR, retStr is updated with new message
		}
		else
		{
			status = COMMAND_UNKNOWN;
		}

		if(status == NO_ACK)
		{
			return;
		}
		else if(status != 0) // other return status other than NO_ACK or NO_ERROR
		{
			PrintError(msg_copy, status);
		}
		else // NO_ERROR
		{
			PrintReply(msg_copy, retStr);
		}
	}
}

void PrintError(char *msg_copy, uint32_t error)
{
	char str[64] = {0};

	GetErrorString(error, str);

	strcat(msg_copy, "=");
	strcat(msg_copy, str);

	UART_IT_IDLE_NotifyUser(&uart2, msg_copy, strlen(msg_copy), true);
}

void PrintReply(char *msg_copy, char *msg2)
{
	strcat(msg_copy, "=");
	strcat(msg_copy, msg2);
	UART_IT_IDLE_NotifyUser(&uart2, msg_copy, strlen(msg_copy), true);
}


/*
 * Description: When HAL receives n amount of bytes or there is an idle state, this is the callback.
 * 				The Size is saved and the queue pointer is incremented. Then UART_IT_IDLE_EnableRxInterrupt is called to enabled UART interrupt again.
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart == uart2.huart)
	{
		uart2.rx.queue[uart2.rx.ptr.index_IN].size = Size;
		RingBuff_Ptr_Input(&uart2.rx.ptr, uart2.rx.queueSize);
		UART_IT_IDLE_EnableRxInterrupt(&uart2);
	}
}

/*
 * Description: When HAL is finished sending, this is the callback.
 * 				Then UART_IT_IDLE_SendMessage is called which will send any pending messages in the queue.
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == uart2.huart)
	{
		UART_IT_IDLE_SendMessage(&uart2);
	}
}

void Blink_LED(void)
{
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}

int HelloWorld(void)
{
	UART_IT_IDLE_NotifyUser(&uart2, "Hello to you too!", strlen("Hello to you too!"), true);
	UART_IT_IDLE_NotifyUser(&uart2, "Add another message for good measure!", strlen("Add another message for good measure!"), true);

	return NO_ACK;
}

/*
 * Description: Parse command to enable or disable blinking of LED
 */
int ParseCMD5(char *msg)
{
	msg += strlen("cmd5:"); // skip cmd5:
	if(atoi(msg) == 0)
	{
		TimerCallbackDisable(&timerCallback, Blink_LED);
	}
	else if(atoi(msg) == 1)
	{
		TimerCallbackTimerStart(&timerCallback, Blink_LED, 500, TIMER_REPEAT);
	}
	else return VALUE_OUT_OF_RANGE;

	return NO_ERROR;
}

int GetVersion(char *msg)
{
	strcpy(msg, version);

	return NO_ERROR;
}
