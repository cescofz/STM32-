/**
	********************************************************************************
	*FileName:  USART.c
	*Author:  石家庄市电子信息学校24级13班李佳澍
	*Version:  1.0.0
	*Date:  2025/10/6
	*Description: 串口驱动
	*********************************************************************************
*/
#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
//数据包发送格式:@MSG\r\n
char USART_RxPacket[100];
uint8_t USART_RxFlag;				    	//数据接收标志位
void Serial_Init(void)	//初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //Tx引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//Pull Up输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	//Rx引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure); 	//初始化
	
	
	USART_InitTypeDef USART_InitStrucrure;
	USART_InitStrucrure.USART_BaudRate = 9600;	//波特率9600
	USART_InitStrucrure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStrucrure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //选择发送和接收模式
	USART_InitStrucrure.USART_Parity = USART_Parity_No;		//无奇偶校验
	USART_InitStrucrure.USART_StopBits = USART_StopBits_1;	//停止位1
	USART_InitStrucrure.USART_WordLength = USART_WordLength_8b;	//8位数据位
	USART_Init(USART1,&USART_InitStrucrure);	//初始化USART1
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);	//开启RX非空中断
	
	//NVIC配置
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStrucrute;
	NVIC_InitStrucrute.NVIC_IRQChannel = USART1_IRQn;	//USART1中断通道
	NVIC_InitStrucrute.NVIC_IRQChannelCmd  = ENABLE;	//启用NVIC通道
	NVIC_InitStrucrute.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级1
	NVIC_InitStrucrute.NVIC_IRQChannelSubPriority = 1;	//响应优先级1
	NVIC_Init(&NVIC_InitStrucrute);	//初始化NVIC
	
	USART_Cmd(USART1,ENABLE);	//启用USART1模块
	
}

void USART_SendByte(uint8_t Byte){ 	//发送字节
	USART_SendData(USART1,Byte); 	//向串口写入数据
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); 	//等待标志位为0,发送完成
}
void USART_SendArray(uint8_t *Array, uint16_t Length){ 	//发送数组
	uint8_t i;
	for(i = 0;i < Length; i++)
	{
		USART_SendByte(Array[i]);	//发送每一位数据
		
	}
}
void USART_SendString(char *string){
	uint8_t i;
	for(i = 0;string[i] != '\0';i++) //发送直到遇到\0字符结束标志位
	{
		USART_SendByte(string[i]);
	}
}
uint32_t USART_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//定义结果值为1
	while (Y --)		//执行Y次
	{
		Result *= X;	//将X累乘到结果
	}
	return Result;
}
void USART_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)	//遍历位数
	{
		USART_SendByte(Number / USART_Pow(10, Length - i - 1) % 10 + '0');
	}
}
void USART_Printf(char *format, ...)
{
	char string[100];			
	va_list arg;				
	va_start(arg, format);			
	vsprintf(string, format, arg);
	va_end(arg);				
	USART_SendString(string);
}

void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{
		uint8_t RxData = USART_ReceiveData(USART1);
		if(RxState == 0)	//等待包头
		{
			if(RxData == '@' && USART_RxFlag == 0)
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if(RxState == 1)	//接收数据
		{
			if(RxData == '\r')
			{
				RxState = 2;
			}
			else 
			{
				USART_RxPacket[pRxPacket] = RxData;
				pRxPacket ++;
			}
		}
		else if(RxState == 2)	//等待第二个包尾
		{
			if(RxData == '\n')
			{
				RxState = 0;
				USART_RxPacket[pRxPacket] = '\0';
				USART_RxFlag = 1;
				
			}
		}
		USART_ClearITPendingBit(USART1,USART_IT_RXNE); 
	}
}
