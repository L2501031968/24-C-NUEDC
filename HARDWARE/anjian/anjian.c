#include "anjian.h"



volatile int reset_idx_flag = 0;
static float Ma = 0.3f; // 新增，调制系数，初始30%
static uint32_t ch3_freq = 30000000; // CH3频率，单位Hz，初始30MHz
static int freq_last_key = 0;
// 相位步进表（以度为单位，后续用时转换）
static const uint16_t phase_table_deg[] = {180, 216, 292, 280, 324, 0};
#define PHASE_TABLE_SIZE (sizeof(phase_table_deg)/sizeof(phase_table_deg[0]))

// 文件作用域静态变量，便于重置
static int Sd_step_idx = 0;
static int Sm_time_delay_phase_idx = 0;
static int Sm_Phase_phase = 0;
//初始化
void chushihua(void)
{
    static uint8_t last_key = 0;
    int key = Button4_4_Scan();
    if(key == 4 && last_key == 0)
    {
// AD9959四路信号输出
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
    IO_Update();

    // 重置所有步进/调制度/相位相关变量
    Sd_step_idx = 0;
    Ma = 0.3f;
    ch3_freq = 30000000;
    freq_last_key = 0;
    Sm_time_delay_phase_idx = 0;
    Sm_Phase_phase = 0;
    reset_idx_flag = 0;
    }
    last_key = key;
}

void CW_OR_AM(void)
{
    static uint8_t ch2_state = 1;
    static uint8_t last_key = 0;
    int key = Button4_4_Scan();
    if(key == 1 && last_key == 0)
    {
        ch2_state = !ch2_state;
        if(ch2_state)
            AD9959_Set_Amp(CH2, 920); // 450mV
        else
            AD9959_Set_Amp(CH2, 0);    // 0mV
        IO_Update();
        delay_ms(20); // 消抖
        IO_Update();
    }
    last_key = key;
}

//Sd幅度步进
#define MAX_STEP 20
void Sd_step(void)
{
    // 常量表，第一列为DAC值，第二列为CH3幅度
    static const uint16_t value_table[][2] = {
        {256, 1023}, {238, 950}, {200, 917}, {170, 881}, {140, 841},
        {105, 796}, {78, 735}, {50, 600}, {40, 400}, {20, 100}
    };
    static int last_key = 0;
    int key = Button4_4_Scan();
    const int max_idx = 9; // 0~9
    int idx = Sd_step_idx;

    if(reset_idx_flag) // 由CW_OR_AM触发
    {
        idx = 0;
        AD9959_Set_Amp(CH3, value_table[idx][1]);
        DAC_SetValue(DAC_CH1, value_table[idx][0]);
        IO_Update();
        delay_ms(20);
        reset_idx_flag = 0;
    }
    else if(key == 2 && last_key == 0) // 下一组
    {
        if(idx < max_idx) idx++;
        AD9959_Set_Amp(CH3, value_table[idx][1]);
        DAC_SetValue(DAC_CH1, value_table[idx][0]);
        IO_Update();
        delay_ms(20);
    }
    else if(key == 3 && last_key == 0) // 上一组
    {
        if(idx > 0) idx--;
        AD9959_Set_Amp(CH3, value_table[idx][1]);
        DAC_SetValue(DAC_CH1, value_table[idx][0]);
        IO_Update();
        delay_ms(20);
    } 
    Sd_step_idx = idx;
    last_key = key;
}

// 调制度调节步进
void Ma_step(void)
{
    static int last_key = 0;
    int key = Button4_4_Scan();
    if(key == 5 && last_key == 0)
    {
        Ma -= 0.1f;
        if(Ma < 0.3f) Ma = 0.3f;
        AD9959_Set_Amp(CH3, 1023); // 载波幅度固定
        AD9959_Set_Amp(CH2, (uint16_t)(1023 * Ma + 0.5f));
        IO_Update();
        delay_ms(20);
    }
    else if(key == 6 && last_key == 0)
    {
        Ma += 0.1f;
        if(Ma > 0.9f) Ma = 0.9f;
        AD9959_Set_Amp(CH3, 1023); // 载波幅度固定
        AD9959_Set_Amp(CH2, (uint16_t)(1023 * Ma + 0.5f));
        IO_Update();
        delay_ms(20);
    }
    last_key = key;
}

//载波频率步进
void Fre_step(void)
{
    int key = Button4_4_Scan();
	if(key == 7 && freq_last_key == 0)
    {
        if(ch3_freq > 30000000) 
			ch3_freq -= 1000000;
        AD9959_Set_Fre(CH3, ch3_freq);
			AD9959_Set_Fre(CH0, ch3_freq);
        IO_Update();
        delay_ms(20);
    }
    
    else if(key == 8 && freq_last_key == 0)
    {
        if(ch3_freq < 40000000) ch3_freq += 1000000;
        AD9959_Set_Fre(CH3, ch3_freq);
        IO_Update();
        delay_ms(20);
    }
    freq_last_key = key;
} 

//多径时延步进
void Sm_time_delay(void)
{
    static int last_key = 0;
    int key = Button4_4_Scan();
    int phase_idx = Sm_time_delay_phase_idx;
    if(key == 9 && last_key == 0) 
    {
        if(phase_idx < PHASE_TABLE_SIZE - 1) phase_idx++;
        uint16_t phase_word = (uint16_t)(phase_table_deg[phase_idx] * 16384 / 360);
        AD9959_Set_Phase(CH0, phase_word);
        IO_Update();
        delay_ms(20);
    }
    else if(key == 10 && last_key == 0) 
    {
        if(phase_idx > 0) phase_idx--;
        uint16_t phase_word = (uint16_t)(phase_table_deg[phase_idx] * 16384 / 360);
        AD9959_Set_Phase(CH0, phase_word);
        IO_Update();
        delay_ms(20);
    }
    Sm_time_delay_phase_idx = phase_idx;
    last_key = key;
}

//PE4302衰减步进
void Sm_PE4302(void)
{
    static uint8_t att_db = 0; // 当前衰减值，单位dB
    static int last_key = 0;
    int key = Button4_4_Scan();
    if(key == 11 && last_key == 0)
    {
        if(att_db < 20) att_db += 2;
        PE4302_SetAttenuationInt(att_db);
        delay_ms(20);
    }
    else if(key == 12 && last_key == 0)
    {
        if(att_db > 0) att_db -= 2;
        PE4302_SetAttenuationInt(att_db);
        delay_ms(20);
    }
    last_key = key;
}

//Sm初相步进
void Sm_Phase(void)
{
    static int last_key = 0;
    int key = Button4_4_Scan();
    int phase = Sm_Phase_phase;
    if(key == 13 && last_key == 0)
    {
        if(phase < 180) phase += 30;
        uint16_t phase_word = (uint16_t)(phase * 16384 / 360);
        AD9959_Set_Phase(CH0, phase_word);
        IO_Update();
        delay_ms(20);
    }
    else if(key == 14 && last_key == 0)
    {
        if(phase > 0) phase -= 30;
        uint16_t phase_word = (uint16_t)(phase * 16384 / 360);
        AD9959_Set_Phase(CH0, phase_word);
        IO_Update();
        delay_ms(20);
    }
    Sm_Phase_phase = phase;
    last_key = key;
}

