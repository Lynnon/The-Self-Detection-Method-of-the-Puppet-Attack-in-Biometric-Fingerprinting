#ifndef  _USART2_H
#define	_USART2_H
#include "stm32f10x.h"
#include "stdio.h"      //包含需要的头文件
#include "stdarg.h"		//包含需要的头文件 
#include "string.h"     //包含需要的头文件

#define USART2_RX_ENABLE  1  //是否开启接收功能  1：开启  0：关闭
#define USART2_TXBUFF_SIZE   1024    //定义串口1 发送缓冲区大小 256字节

#if USART2_RX_ENABLE==1
#define USART2_RXBUFF_SIZE  1024              //定义串口1 接收缓冲区大小 256字节
extern char Usart2_RxCompleted ;               //外部声明，其他文件可以调用该变量
extern unsigned int Usart2_RxCounter;          //外部声明，其他文件可以调用该变量
extern char Usart2_RxBuff[USART2_RXBUFF_SIZE]; //外部声明，其他文件可以调用该变量
#endif


void Usart2_Init(unsigned int bound2);
void u2_printf(char* fmt,...) ;
void USART2_IRQHandler(void) ;

#endif

