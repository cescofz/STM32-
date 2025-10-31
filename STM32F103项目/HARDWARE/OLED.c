/**
	********************************************************************************
	*FileName:  OLED.c
	*Author:  石家庄市电子信息学校24级13班李佳澍
	*Version:  1.0.0
	*Date:  2025/10/6
	*Description: 0.96寸OLED显示屏硬件I2C驱动文件(移植江科大STM32教程1-4节的软件驱动OLED程序)
	*********************************************************************************
*/
/* 硬件I2C驱动详细注释见SHT30.c
	硬件I2C驱动，请务必正确连接
	SCL -> PB6
	SDA -> PB7
*/
#include "stm32f10x.h"                  // Device header
#include "OLED_Font.h"
#define I2C_ADDRESS 0x78

void OLED_I2C_Init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	
	I2C_Init(I2C1,&I2C_InitStructure);
	I2C_Cmd(I2C1,ENABLE);
}

static void I2C_WaitEvent(uint32_t I2C_EVENT)
{
    uint32_t Timeout = 100000;
    while (I2C_CheckEvent(I2C1, I2C_EVENT) != SUCCESS)
    {
        if (Timeout-- == 0) break;
    }
}

void I2C_SendByte(uint8_t byte)
{
	I2C_SendData(I2C1,byte);
	I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED);
}

void I2C_Send_Cmd(uint8_t cmd)
{
    I2C_GenerateSTART(I2C1, ENABLE);
    I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT);
    
    I2C_SendData(I2C1, I2C_ADDRESS);
    I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
    
    I2C_SendData(I2C1, 0x00);      // 写入命令
    I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED);
    
    I2C_SendData(I2C1, cmd);
    I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED);
    
    I2C_GenerateSTOP(I2C1, ENABLE);
}

void I2C_WriteData(const uint8_t *Data, uint16_t Count)
{
	uint16_t i;
    I2C_GenerateSTART(I2C1, ENABLE);
    I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT);
    
    I2C_SendData(I2C1, I2C_ADDRESS);
    I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	I2C_SendByte(0x40);
    I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED);
    for (i = 0; i < Count; i++)
    {
        I2C_SendData(I2C1, Data[i]);
        I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED);
    }
	I2C_GenerateSTOP(I2C1, ENABLE);
}

void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	I2C_Send_Cmd(0xB0 | Y);				//设置Y位置
	I2C_Send_Cmd(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	I2C_Send_Cmd(0x00 | (X & 0x0F));		//设置X位置低4位
}

void OLED_Clear(void)
{  
    uint8_t j;
    uint8_t zero_buffer[128] = {0};  // 全零缓冲区
    
    for (j = 0; j < 8; j++)
    {
        OLED_SetCursor(j, 0);
        I2C_WriteData(zero_buffer, 128);  // 一次写入整行128字节
    }
}

void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{       
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);
    I2C_WriteData(&OLED_F8x16[Char - ' '][0], 8);      // 上半部分
    
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);
    I2C_WriteData(&OLED_F8x16[Char - ' '][8], 8);      // 下半部分
}

uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)					
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)					
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)					
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)					
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

void OLED_ShowChineseChar(uint8_t Line, uint8_t Column, uint8_t ChineseChar)
{
    // 16x16汉字占两行高度，横向占16列
    uint8_t start_page = (Line - 1) * 2;      // 起始页地址
    uint8_t start_col = (Column - 1) * 16;    // 起始列地址
    
    // 显示上半部分（前16字节）
    OLED_SetCursor(start_page, start_col);
    I2C_WriteData(&OLED_F16x16[ChineseChar][0], 16);
    
    // 显示下半部分（后16字节）
    OLED_SetCursor(start_page + 1, start_col);
    I2C_WriteData(&OLED_F16x16[ChineseChar][16], 16);
}

void OLED_ShowChineseString(uint8_t Line, uint8_t Column, uint8_t *ChineseString)
{
    uint8_t i;
    for (i = 0; ChineseString[i] != 0; i++)
    {
        OLED_ShowChineseChar(Line, Column + i, ChineseString[i]);
    }
}
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//开机延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//端口初始化
	
	I2C_Send_Cmd(0xAE);	//关闭显示
	
	I2C_Send_Cmd(0xD5);	//设置显示时钟分频/振荡器频率
	I2C_Send_Cmd(0x80);
	
	I2C_Send_Cmd(0xA8);	//设置多路复用率
	I2C_Send_Cmd(0x3F);
	
	I2C_Send_Cmd(0xD3);	//设置显示偏移
	I2C_Send_Cmd(0x00);
	
	I2C_Send_Cmd(0x40);	//设置显示起始行
	
	I2C_Send_Cmd(0xA1);	//设置左右方向0xA1正常 0xA0左右反
	
	I2C_Send_Cmd(0xC8);	//设置上下方向0xC8正常 0xC0上下反

	I2C_Send_Cmd(0xDA);	//设置COM引脚硬件配置
	I2C_Send_Cmd(0x12);
	
	I2C_Send_Cmd(0x81);	//设置对比度控制
	I2C_Send_Cmd(0xCF);

	I2C_Send_Cmd(0xD9);	//设置预充电周期
	I2C_Send_Cmd(0xF1);

	I2C_Send_Cmd(0xDB);	//设置VCOMH电压选择设置
	I2C_Send_Cmd(0x30);

	I2C_Send_Cmd(0xA4);	//设置显示全部显示/关闭

	I2C_Send_Cmd(0xA6);	//设置正常/反相显示

	I2C_Send_Cmd(0x8D);	//设置充电
	I2C_Send_Cmd(0x14);

	I2C_Send_Cmd(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}
