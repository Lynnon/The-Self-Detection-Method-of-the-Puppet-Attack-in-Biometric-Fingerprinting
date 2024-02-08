#ifndef __USART1_H
#define __USART1_H	 
#include "sys.h"  

#define USART1_MAX_RECV_LEN	  9000					//�����ջ����ֽ���
#define USART1_MAX_SEND_LEN		400					//����ͻ����ֽ���
#define USART1_RX_EN 			1					//0,������;1,����.

extern u8  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//���ջ���,���USART1_MAX_RECV_LEN�ֽ�
extern u8  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 		//���ͻ���,���USART1_MAX_SEND_LEN�ֽ�
extern vu16 USART1_RX_STA;   						//��������״̬

extern u8 USART1_2_RX_BUF[USART1_MAX_RECV_LEN];

void USART1_init(u32 bound);				//����3��ʼ�� 
void u1_printf(char* fmt,...);
#endif













