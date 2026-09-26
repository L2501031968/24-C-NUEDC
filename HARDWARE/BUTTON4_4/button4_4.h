#ifndef __BUTTON4_4_H
#define	__BUTTON4_4_H
#include "stm32f10x.h"
#include "delay.h"
#include "math.h"



#define BUTTON_GPIO_CLK  RCC_APB2Periph_GPIOF

#define BUTTON_ROW1_GPIO_PORT   GPIOF
#define BUTTON_ROW1_GPIO_PIN    GPIO_Pin_4
#define BUTTON_ROW2_GPIO_PORT   GPIOF
#define BUTTON_ROW2_GPIO_PIN    GPIO_Pin_5
#define BUTTON_ROW3_GPIO_PORT   GPIOF
#define BUTTON_ROW3_GPIO_PIN    GPIO_Pin_6
#define BUTTON_ROW4_GPIO_PORT   GPIOF
#define BUTTON_ROW4_GPIO_PIN    GPIO_Pin_7

#define BUTTON_COL1_GPIO_PORT   GPIOF
#define BUTTON_COL1_GPIO_PIN    GPIO_Pin_0
#define BUTTON_COL2_GPIO_PORT   GPIOF
#define BUTTON_COL2_GPIO_PIN    GPIO_Pin_1
#define BUTTON_COL3_GPIO_PORT   GPIOF
#define BUTTON_COL3_GPIO_PIN    GPIO_Pin_2
#define BUTTON_COL4_GPIO_PORT   GPIOF
#define BUTTON_COL4_GPIO_PIN    GPIO_Pin_3

	
/*********************END**********************/

void Button4_4_Init(void);
int Button4_4_Scan(void);


#endif

