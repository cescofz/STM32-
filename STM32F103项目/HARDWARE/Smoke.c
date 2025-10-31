/**	********************************************************************************
	*FileName:  main.c
	*Author:  石家庄市电子信息学校24级13班李佳澍
	*Version:  1.0.0
	*DateL 2025/10/20
	*Description: 烟雾传感器驱动
	*********************************************************************************
*/
#include "stm32f10x.h"                  // Device header

void SMOKE_GPIO_INIT(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStrcuture;
	GPIO_InitStrcuture.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStrcuture.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStrcuture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStrcuture);
}
uint8_t GET_SMOKE_STATE(void)
{
	if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) == 0)
	{
		return 1;
	}
	if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) == 1)
	{
		return 0;
	}
	else{
		return 0;
	}
}