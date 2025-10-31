/**
	********************************************************************************
	*FileName:  LED.C
	*Author:  石家庄市电子信息学校24级13班李佳澍
	*Version:  1.0.0
	*Date:  2025/10/6
	*Description: LED驱动
	*********************************************************************************
*/
#include "LED.h"
#include "stm32f10x.h"                  // Device header

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
}
void LED_ON(void)

{
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
}
void LED_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
}
void LED_Toggle(void)
{
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_2) == 0)			//读取当前的输出状态，如果当前输出的是低电平
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_2);						//将端口PA1设置为高电平
	}
	else														//否则，说明当前输出高电平
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);						//将端口PA1设置为低电平
	}
}