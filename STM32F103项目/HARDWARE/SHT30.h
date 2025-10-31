#ifndef __SHT30_H__
#define __SHT30_H__
#include "stm32f10x.h"                  // Device header

void SHT30_Init();
void SHT30_IIC_Init();
uint8_t SHT30_ReadData(float *temp, float *hum);
static void SHT30_SendCmd(uint8_t cmd_h,uint8_t cmd_l);
static uint8_t SHT30_CheckCrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum); //CRCÐ£Ñé

#endif