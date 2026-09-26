#ifndef __PE4302_H
#define __PE4302_H

#include "stm32f10x.h"

// 函数声明
void PE4302_Init(void);
int8_t PE4302_SetAttenuation(float dB);
int8_t PE4302_SetAttenuationInt(uint8_t dB);
uint8_t PE4302_GetControlBits(float dB);

#endif

