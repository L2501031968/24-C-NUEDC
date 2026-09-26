#ifndef __DAC_H
#define __DAC_H

#include "stm32f10x.h"

// DAC通道枚举，方便后续扩展
typedef enum {
    DAC_CH1 = 1,
    DAC_CH2 = 2
} DAC_Channel_t;

void MyDAC_Init(DAC_Channel_t channel);
void DAC_SetValue(DAC_Channel_t channel, uint16_t value);

#endif 
