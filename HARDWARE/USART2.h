#ifndef  _USART2_H
#define	_USART2_H
#include "stm32f10x.h"
#include "stdio.h"      //������Ҫ��ͷ�ļ�
#include "stdarg.h"		//������Ҫ��ͷ�ļ� 
#include "string.h"     //������Ҫ��ͷ�ļ�

#define USART2_RX_ENABLE  1  //�Ƿ������չ���  1������  0���ر�
#define USART2_TXBUFF_SIZE   1024    //���崮��1 ���ͻ�������С 256�ֽ�

#if USART2_RX_ENABLE==1
#define USART2_RXBUFF_SIZE  1024              //���崮��1 ���ջ�������С 256�ֽ�
extern char Usart2_RxCompleted ;               //�ⲿ�����������ļ����Ե��øñ���
extern unsigned int Usart2_RxCounter;          //�ⲿ�����������ļ����Ե��øñ���
extern char Usart2_RxBuff[USART2_RXBUFF_SIZE]; //�ⲿ�����������ļ����Ե��øñ���
#endif


void Usart2_Init(unsigned int bound2);
void u2_printf(char* fmt,...) ;
void USART2_IRQHandler(void) ;

#endif

