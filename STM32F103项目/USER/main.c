/**	********************************************************************************
	*FileName:  main.c
	*Author:  石家庄市电子信息学校24级13班李佳澍     初学者，耗时15天开发   
	*Version:  1.0.0
	*DateL 2025/10/20
	*Description: 主文件
	Please open this file using UTF-8 encoding!!!
	此项目实现STM32读取SHT30温湿度传感器、烟雾传感器数据并将数据用0.96寸OLED显示屏显示。
	当烟雾传感器检测到烟雾时控制继电器吸合，继电器可以外接报警设备。
	传感器数据经过cJSON库处理为JSON用USART串口发送给ESP32并使用MQTT协议上报至新大陆物联网云平台(https://api.nlecloud.com/)该平台提供开发者免费注册使用 。
	可以实现手机APP调用API实时检测传感器数据
	云平台账号:15511896620
	云平台密码:123456
	项目ID:1233183
	ESP32设备ID :1331745
	ESP32设备标识符:SJZSTM32MQTT
	OLED显示字段说明:
	Temperature: 温度 单位:℃
	Humidity: 湿度 单位:%
	Smoke: 烟雾状态 0 是没烟 1是有烟
	Cloud: 云平台连接状态 Connection:连接 Disconnect断开连接
	
	*********************************************************************************
	硬件连接:
	所有设备使用统一供电，不得高于3.3V，不要反接!
	野火小智开发板接STLINK:
	3V3 -> VERF
	TMS -> SWDIO
	GND -> GND
	TCK -> SWCLK
	RST -> RST
	
	SHT30温湿度:
	VCC -> +3.3V
	GND -> GND
	SCL -> B10
	SDA -> B11
	
	OLED显示屏:
	VCC -> +3.3V
	GND -> GND
	SCL -> B6
	SDA -> B7
	
	烟雾传感器:
	VCC -> +3.3V
	GND -> GND
	DO -> A5
	
	继电器(低电平触发)：
	DC+ -> +3.3V
	DC- -> GND
	IN1 -> A6
	IN2 -> A7
	
	LED:
	野火小智开发板板载LED A2
	按键:
	野火小智开发板板载按键 A0
	
	接ESP32的串口
	A9(TX) -> ESP32的D17(RX)
	A10(RX) -> ESP32的D16(TX)
	串口配置:
	波特率:9600
	数据位:8
	停止位:1
	奇偶校验:无
	发送编码:UTF-8
	*********************************************************************************
*/

#include "stm32f10x.h"                  // Device header
#include "USART.h"
#include "Key.h"
#include "Delay.h"
#include "string.h"
#include <stdio.h>
#include "SHT30.h"
#include "LED.h"
#include "OLED.h"
#include "Relay.h"
#include "cJSON.h"
#include "Smoke.h"
uint8_t adcData[4];
void HardWare_Init(void)
{
	OLED_Init();
	OLED_ShowString(1,1,"Initializing...");
	Serial_Init();
	SHT30_Init();
	Relay_Init();
	LED_Init();
	SMOKE_GPIO_INIT();
	
}
void Sensor(void)
{
	float temp,hum;
	uint8_t smoke_state = GET_SMOKE_STATE();
	cJSON *root;
	root=cJSON_CreateObject(); 
	if(smoke_state == 1)
	{
		Relay_ON(1);
	}
	else
	{
		Relay_OFF(1);
	}
	if(SHT30_ReadData(&temp,&hum) == 0)
	{
		OLED_ShowNum(1,13,(int)temp,2);
		OLED_ShowNum(2,10,(int)hum,2);
		OLED_ShowNum(3,7,smoke_state,1);
		cJSON_AddNumberToObject(root,"temperature",(int)temp);
		cJSON_AddNumberToObject(root,"humidity",(int)hum);
		cJSON_AddNumberToObject(root,"smoke",(int)smoke_state);
		char *json_str = cJSON_Print(root);
		USART_SendString(json_str);
		
		cJSON_Delete(root);
		cJSON_free(json_str);
		Delay_ms(1000);
	}

}
int main()
{
	float temp,hum;
	uint16_t connect_state;
	HardWare_Init();
	Delay_s(1);
	OLED_Clear();
	OLED_ShowString(1,1,"Temperature:");
	OLED_ShowString(2,1,"Humidity:");
	OLED_ShowString(2,12,"%");
	OLED_ShowString(4,1,"Cloud:");
	OLED_ShowString(3,1,"Smoke:");
	while(1)
	{
		Sensor();
		if (USART_RxFlag == 1)
		{
			
			if(strcmp(&USART_RxPacket[0],"Connection") == 0)
			{
				connect_state = 1;
			}
			if(strcmp(&USART_RxPacket[0],"Disconnect") == 0)
			{
				connect_state = 2;
			}
			USART_RxFlag = 0;
		}
		if (connect_state == 1)
		{
			OLED_ShowString(4,7,"Connection");
		}
		if (connect_state == 0 || connect_state == 2)
		{
			OLED_ShowString(4,7,"Disconnect");
		}
	}
}