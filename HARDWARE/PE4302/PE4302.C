
// PE4302.c
// STM32F103RCT6 控制 PE4302 数控衰减器

#include "PE4302.H"
#include <stdint.h>
#include <string.h>

// PE4302控制引脚定义
#define PE4302_C16_PIN    GPIO_Pin_0  // PE0 - 16dB控制位
#define PE4302_C8_PIN     GPIO_Pin_1  // PE1 - 8dB控制位  
#define PE4302_C4_PIN     GPIO_Pin_2  // PE2 - 4dB控制位
#define PE4302_C2_PIN     GPIO_Pin_3  // PE3 - 2dB控制位
#define PE4302_C1_PIN     GPIO_Pin_4  // PE4 - 1dB控制位
#define PE4302_C0_5_PIN   GPIO_Pin_5  // PE5 - 0.5dB控制位

#define PE4302_GPIO_PORT  GPIOE

// 衰减状态结构体
typedef struct {
    float attenuation_db; // 衰减量（dB）
    uint8_t C16;
    uint8_t C8;
    uint8_t C4;
    uint8_t C2;
    uint8_t C1;
    uint8_t C0_5;
} PE4302_AttenuationState;

// 衰减状态表 - 根据PE4302数据手册
const PE4302_AttenuationState PE4302_table[] = {
    {0.0f,   0, 0, 0, 0, 0, 0},   // Reference Loss
    {0.5f,   0, 0, 0, 0, 0, 1},   // 0.5 dB
    {1.0f,   0, 0, 0, 0, 1, 0},   // 1 dB
    {1.5f,   0, 0, 0, 0, 1, 1},   // 1.5 dB
    {2.0f,   0, 0, 0, 1, 0, 0},   // 2 dB
    {2.5f,   0, 0, 0, 1, 0, 1},   // 2.5 dB
    {3.0f,   0, 0, 0, 1, 1, 0},   // 3 dB
    {3.5f,   0, 0, 0, 1, 1, 1},   // 3.5 dB
    {4.0f,   0, 0, 1, 0, 0, 0},   // 4 dB
    {4.5f,   0, 0, 1, 0, 0, 1},   // 4.5 dB
    {5.0f,   0, 0, 1, 0, 1, 0},   // 5 dB
    {5.5f,   0, 0, 1, 0, 1, 1},   // 5.5 dB
    {6.0f,   0, 0, 1, 1, 0, 0},   // 6 dB
    {6.5f,   0, 0, 1, 1, 0, 1},   // 6.5 dB
    {7.0f,   0, 0, 1, 1, 1, 0},   // 7 dB
    {7.5f,   0, 0, 1, 1, 1, 1},   // 7.5 dB
    {8.0f,   0, 1, 0, 0, 0, 0},   // 8 dB
    {8.5f,   0, 1, 0, 0, 0, 1},   // 8.5 dB
    {9.0f,   0, 1, 0, 0, 1, 0},   // 9 dB
    {9.5f,   0, 1, 0, 0, 1, 1},   // 9.5 dB
    {10.0f,  0, 1, 0, 1, 0, 0},   // 10 dB
    {10.5f,  0, 1, 0, 1, 0, 1},   // 10.5 dB
    {11.0f,  0, 1, 0, 1, 1, 0},   // 11 dB
    {11.5f,  0, 1, 0, 1, 1, 1},   // 11.5 dB
    {12.0f,  0, 1, 1, 0, 0, 0},   // 12 dB
    {12.5f,  0, 1, 1, 0, 0, 1},   // 12.5 dB
    {13.0f,  0, 1, 1, 0, 1, 0},   // 13 dB
    {13.5f,  0, 1, 1, 0, 1, 1},   // 13.5 dB
    {14.0f,  0, 1, 1, 1, 0, 0},   // 14 dB
    {14.5f,  0, 1, 1, 1, 0, 1},   // 14.5 dB
    {15.0f,  0, 1, 1, 1, 1, 0},   // 15 dB
    {15.5f,  0, 1, 1, 1, 1, 1},   // 15.5 dB
    {16.0f,  1, 0, 0, 0, 0, 0},   // 16 dB
    {16.5f,  1, 0, 0, 0, 0, 1},   // 16.5 dB
    {17.0f,  1, 0, 0, 0, 1, 0},   // 17 dB
    {17.5f,  1, 0, 0, 0, 1, 1},   // 17.5 dB
    {18.0f,  1, 0, 0, 1, 0, 0},   // 18 dB
    {18.5f,  1, 0, 0, 1, 0, 1},   // 18.5 dB
    {19.0f,  1, 0, 0, 1, 1, 0},   // 19 dB
    {19.5f,  1, 0, 0, 1, 1, 1},   // 19.5 dB
    {20.0f,  1, 0, 1, 0, 0, 0},   // 20 dB
    {20.5f,  1, 0, 1, 0, 0, 1},   // 20.5 dB
    {21.0f,  1, 0, 1, 0, 1, 0},   // 21 dB
    {21.5f,  1, 0, 1, 0, 1, 1},   // 21.5 dB
    {22.0f,  1, 0, 1, 1, 0, 0},   // 22 dB
    {22.5f,  1, 0, 1, 1, 0, 1},   // 22.5 dB
    {23.0f,  1, 0, 1, 1, 1, 0},   // 23 dB
    {23.5f,  1, 0, 1, 1, 1, 1},   // 23.5 dB
    {24.0f,  1, 1, 0, 0, 0, 0},   // 24 dB
    {24.5f,  1, 1, 0, 0, 0, 1},   // 24.5 dB
    {25.0f,  1, 1, 0, 0, 1, 0},   // 25 dB
    {25.5f,  1, 1, 0, 0, 1, 1},   // 25.5 dB
    {26.0f,  1, 1, 0, 1, 0, 0},   // 26 dB
    {26.5f,  1, 1, 0, 1, 0, 1},   // 26.5 dB
    {27.0f,  1, 1, 0, 1, 1, 0},   // 27 dB
    {27.5f,  1, 1, 0, 1, 1, 1},   // 27.5 dB
    {28.0f,  1, 1, 1, 0, 0, 0},   // 28 dB
    {28.5f,  1, 1, 1, 0, 0, 1},   // 28.5 dB
    {29.0f,  1, 1, 1, 0, 1, 0},   // 29 dB
    {29.5f,  1, 1, 1, 0, 1, 1},   // 29.5 dB
    {30.0f,  1, 1, 1, 1, 0, 0},   // 30 dB
    {30.5f,  1, 1, 1, 1, 0, 1},   // 30.5 dB
    {31.0f,  1, 1, 1, 1, 1, 0},   // 31 dB
    {31.5f,  1, 1, 1, 1, 1, 1}    // 31.5 dB
};

// 表的长度
const uint8_t PE4302_table_len = sizeof(PE4302_table) / sizeof(PE4302_table[0]);

/**
 * @brief PE4302初始化函数
 * @param None
 * @retval None
 */
void PE4302_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    
    // 配置PE4302控制引脚为推挽输出
    GPIO_InitStructure.GPIO_Pin = PE4302_C16_PIN | PE4302_C8_PIN | PE4302_C4_PIN | 
                                  PE4302_C2_PIN | PE4302_C1_PIN | PE4302_C0_5_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PE4302_GPIO_PORT, &GPIO_InitStructure);
    
    // 初始化所有引脚为低电平（0dB衰减）
    GPIO_ResetBits(PE4302_GPIO_PORT, PE4302_C16_PIN | PE4302_C8_PIN | PE4302_C4_PIN | 
                                   PE4302_C2_PIN | PE4302_C1_PIN | PE4302_C0_5_PIN);
}

/**
 * @brief 设置PE4302衰减值（浮点数版本）
 * @param dB: 衰减值（0.0-31.5 dB）
 * @retval 0: 成功, -1: 失败
 */
int8_t PE4302_SetAttenuation(float dB)
{
    uint8_t i;
    uint16_t control_bits = 0;
    
    // 检查输入范围
    if (dB < 0.0f || dB > 31.5f) {
        return -1;
    }
    
    // 查找最接近的衰减值
    for (i = 0; i < PE4302_table_len; i++) {
        if (PE4302_table[i].attenuation_db >= dB) {
            break;
        }
    }
    
    // 如果超出范围，使用最大值
    if (i >= PE4302_table_len) {
        i = PE4302_table_len - 1;
    }
    
    // 构建控制位
    control_bits = (PE4302_table[i].C16 << 5) | (PE4302_table[i].C8 << 4) | 
                   (PE4302_table[i].C4 << 3) | (PE4302_table[i].C2 << 2) | 
                   (PE4302_table[i].C1 << 1) | PE4302_table[i].C0_5;
    
    // 设置GPIO引脚状态
    if (PE4302_table[i].C16) GPIO_SetBits(PE4302_GPIO_PORT, PE4302_C16_PIN);
    else GPIO_ResetBits(PE4302_GPIO_PORT, PE4302_C16_PIN);
    
    if (PE4302_table[i].C8) GPIO_SetBits(PE4302_GPIO_PORT, PE4302_C8_PIN);
    else GPIO_ResetBits(PE4302_GPIO_PORT, PE4302_C8_PIN);
    
    if (PE4302_table[i].C4) GPIO_SetBits(PE4302_GPIO_PORT, PE4302_C4_PIN);
    else GPIO_ResetBits(PE4302_GPIO_PORT, PE4302_C4_PIN);
    
    if (PE4302_table[i].C2) GPIO_SetBits(PE4302_GPIO_PORT, PE4302_C2_PIN);
    else GPIO_ResetBits(PE4302_GPIO_PORT, PE4302_C2_PIN);
    
    if (PE4302_table[i].C1) GPIO_SetBits(PE4302_GPIO_PORT, PE4302_C1_PIN);
    else GPIO_ResetBits(PE4302_GPIO_PORT, PE4302_C1_PIN);
    
    if (PE4302_table[i].C0_5) GPIO_SetBits(PE4302_GPIO_PORT, PE4302_C0_5_PIN);
    else GPIO_ResetBits(PE4302_GPIO_PORT, PE4302_C0_5_PIN);
    
    return 0;
}

/**
 * @brief 设置PE4302衰减值（整数版本，方便调用）
 * @param dB: 衰减值（0-31 dB，整数）
 * @retval 0: 成功, -1: 失败
 */
int8_t PE4302_SetAttenuationInt(uint8_t dB)
{
    return PE4302_SetAttenuation((float)dB);
}

/**
 * @brief 获取当前衰减值对应的控制位
 * @param dB: 衰减值
 * @retval 控制位（6位，从高位到低位：C16,C8,C4,C2,C1,C0.5）
 */
uint8_t PE4302_GetControlBits(float dB)
{
    uint8_t i;
    
    // 查找最接近的衰减值
    for (i = 0; i < PE4302_table_len; i++) {
        if (PE4302_table[i].attenuation_db >= dB) {
            break;
        }
    }
    
    // 如果超出范围，使用最大值
    if (i >= PE4302_table_len) {
        i = PE4302_table_len - 1;
    }
    
    // 返回控制位
    return (PE4302_table[i].C16 << 5) | (PE4302_table[i].C8 << 4) | 
           (PE4302_table[i].C4 << 3) | (PE4302_table[i].C2 << 2) | 
           (PE4302_table[i].C1 << 1) | PE4302_table[i].C0_5;
} 
