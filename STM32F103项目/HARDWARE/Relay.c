/**	********************************************************************************
	*FileName:  main.c
	*Author:  石家庄市电子信息学校24级13班李佳澍
	*Version:  1.0.0
	*Date:  2025/10/6
	*Description: 继电器驱动
	*********************************************************************************
*/
#include "stm32f10x.h"                  // Device header

void Relay_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
}
void Relay_ON(uint8_t IN)
{
	if (IN == 1)
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	}
	if (IN == 2)
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	}
}
void Relay_OFF(uint8_t IN)
{
	if (IN == 1)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_6);
	}
	if (IN == 2)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_7);
	}
}
void Relay_Toggle(void)
{
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) == 0)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_6);
	}
	else{
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7) == 0)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_7);
	}
	else{
		GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	}
}