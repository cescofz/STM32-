#ifndef __RELAY_H__
#define __RELAY_H__
#include "stm32f10x.h"                  // Device header
void Relay_Init();
void Relay_ON(uint8_t IN);
void Relay_OFF(uint8_t IN);
void Relay_Toggle();
#endif