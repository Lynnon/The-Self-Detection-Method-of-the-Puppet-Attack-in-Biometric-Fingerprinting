#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"	 


#define KEY1 PCin(13)
#define KEY2 PCin(14) 
#define KEY3 PCin(15)
#define KEY4 PAin(1)
#define KEY5 PAin(0)

#define KEY1_PORT_CLK  RCC_APB2Periph_GPIOC
#define KEY1_PORT			 GPIOC
#define KEY1_PORT_PIN  GPIO_Pin_13

#define KEY2_PORT_CLK  RCC_APB2Periph_GPIOC
#define KEY2_PORT			 GPIOC
#define KEY2_PORT_PIN  GPIO_Pin_14

#define KEY3_PORT_CLK  RCC_APB2Periph_GPIOC
#define KEY3_PORT			 GPIOC
#define KEY3_PORT_PIN  GPIO_Pin_15

#define KEY4_PORT_CLK  RCC_APB2Periph_GPIOA
#define KEY4_PORT			 GPIOA
#define KEY4_PORT_PIN  GPIO_Pin_1

#define KEY5_PORT_CLK  RCC_APB2Periph_GPIOA
#define KEY5_PORT			 GPIOA
#define KEY5_PORT_PIN  GPIO_Pin_0

extern u8 key_num;

void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8 mode);//按键扫描函数					    
#endif
