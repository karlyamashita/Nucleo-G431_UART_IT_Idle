/*
 * PollingRoutine.h
 *
 *  Created on: Oct 24, 2023
 *      Author: karl.yamashita
 *
 *
 *      Template
 */

#ifndef INC_POLLINGROUTINE_H_
#define INC_POLLINGROUTINE_H_


/*

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <ctype.h>
#define Nop() asm(" NOP ")

*/


void PollingInit(void);
void PollingRoutine(void);

void UART2_Parse(UART_IT_IDLE_Queue_t *msg);
void Blink_LED(void);

void PrintError(char *msg_copy, uint32_t error);
void PrintReply(char *msg_copy, char *msg2);

int HelloWorld(void);
int ParseCMD5(char *msg);
int GetVersion(char *msg);

#endif /* INC_POLLINGROUTINE_H_ */
