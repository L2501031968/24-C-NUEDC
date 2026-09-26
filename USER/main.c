// main.c
// STM32F103RCT6 主程序 - AD9959四路信号输出示例

#include "stm32f10x.h"
#include "PE4302.H"
#include "dac.h"
#include <stdio.h>
#include "stm32_config.h"
#include "AD9959.h"
#include "button4_4.h" // 加载矩阵键盘头文件
#include "anjian.h" // 新增，按键控制AD9959相关操作
#include "test.h"
#include "lcd.h"
#include "gui.h"
int main(void)
{
    SystemInit();
    MY_NVIC_PriorityGroup_Config(NVIC_PriorityGroup_2); // 设置中断分组
    delay_init(72); // 初始化延时函数
    delay_ms(1000); // 延时一会儿，等待上电稳定,确保AD9959比控制板先上电。

    Button4_4_Init(); // 初始化矩阵键盘
    PE4302_Init(); // 初始化PE4302
	  //LCD_Init();	   //液晶屏初始化
	  //LCD_direction(1);
    MyDAC_Init(DAC_CH1); // 初始化DAC通道1（PA4）
    MyDAC_Init(DAC_CH2); // 初始化DAC通道2（PA5）
    DAC_SetValue(DAC_CH1, 20); // 输出中间电压
	  DAC_SetValue(DAC_CH2, 20); // 输出中间电压


    // AD9959四路信号输出
    AD9959_Init(); // 初始化AD9959
    AD9959_Set_Fre(CH0, 30000000); // 通道0 30MHz
    AD9959_Set_Fre(CH1, 2000000);  // 通道1 2MHz
    AD9959_Set_Fre(CH2, 2000000);  // 通道2 2MHz
    AD9959_Set_Fre(CH3, 30000000); // 通道3 30MHz

    AD9959_Set_Amp(CH0, 1023); // 通道0 500mV（满幅）
    AD9959_Set_Amp(CH1, 307);  // 通道1 150mV
    AD9959_Set_Amp(CH2, 307); // 通道2 500mV（初始为500mV）
    AD9959_Set_Amp(CH3, 1023); // 通道3 500mV（满幅）

    AD9959_Set_Phase(CH0, 0);
    AD9959_Set_Phase(CH1, 0);
    AD9959_Set_Phase(CH2, 0);
    AD9959_Set_Phase(CH3, 0);

    IO_Update(); // AD9959更新数据
    //Pic_test();
    while(1)
    {
      chushihua();//初始化
			CW_OR_AM(); // 按键S1控制CH2通道0/500mV切换
			Sd_step();
		  Ma_step();
			Fre_step();
      Sm_time_delay();
      Sm_PE4302();
      Sm_Phase(); // 
			//Rotate_Test();

        // 可在此处继续操作PE4302、DAC等
        // PE4302_SetAttenuationInt(8); // 示例：设置8dB衰减
    }
}

// PE4302控制引脚：PE0~PE5
// DAC通道1输出：PA4
// DAC_SetValue(DAC_CH1, value)  // value范围0~4095 
// CH1、CH2：2MHz，幅度150mV，调制波
// CH0、CH3：30MHz，幅度500mV，载波 
// 矩阵键盘C4-C1 R1-R4:PF0-PF7
// LCD屏幕管脚 SDI-PB6,SDO-PB7,LED-PB9,SCK-PB5,DC/RS-PB8,RST-PB14,CS-PB15；
