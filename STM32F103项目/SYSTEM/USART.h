#ifndef __USART_H__
#define __USART_H__                // Device header
#include <stdio.h>
extern  char USART_RxPacket[100];
extern uint8_t USART_RxFlag;					//数据接收标志位
void Serial_Init(void);
void USART_SendByte(uint8_t Byte);
void USART_SendArray(uint8_t *Array, uint16_t Length);
void USART_SendString(char *string);
void USART_SendNumber(uint32_t Number, uint8_t Length);
void USART_Printf(char *format, ...);
#endif