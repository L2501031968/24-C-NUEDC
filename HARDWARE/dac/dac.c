#include "dac.h"

void MyDAC_Init(DAC_Channel_t channel)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef  DAC_InitStructure;

    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    // 配置DAC通道对应的GPIO为模拟输入
    if(channel == DAC_CH1) {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; // DAC_OUT1 -> PA4
    } else if(channel == DAC_CH2) {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; // DAC_OUT2 -> PA5
    }
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // DAC配置
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_None; // 软件触发
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

    if(channel == DAC_CH1) {
        DAC_Init(DAC_Channel_1, &DAC_InitStructure);
        DAC_Cmd(DAC_Channel_1, ENABLE);
    } else if(channel == DAC_CH2) {
        DAC_Init(DAC_Channel_2, &DAC_InitStructure);
        DAC_Cmd(DAC_Channel_2, ENABLE);
    }
}

void DAC_SetValue(DAC_Channel_t channel, uint16_t value)
{
    if(channel == DAC_CH1) {
        DAC_SetChannel1Data(DAC_Align_12b_R, value);
    } else if(channel == DAC_CH2) {
        DAC_SetChannel2Data(DAC_Align_12b_R, value);
    }
} 
