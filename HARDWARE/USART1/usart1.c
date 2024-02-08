#include "delay.h"
#include "USART1.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "timer.h"
  

//���ڽ��ջ����� 	
u8 USART1_RX_BUF[USART1_MAX_RECV_LEN]; 				//���ջ���,���USART1_MAX_RECV_LEN���ֽ�.
u8 USART1_TX_BUF[USART1_MAX_SEND_LEN]; 			  //���ͻ���,���USART1_MAX_SEND_LEN�ֽ�

//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
vu16 USART1_RX_STA=0;   	

u8 USART1_2_RX_BUF[USART1_MAX_RECV_LEN];


void USART1_IRQHandler(void)
{
	u8 res;	      
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		res =USART_ReceiveData(USART1);		 
		if((USART1_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
		{ 
			if(USART1_RX_STA<USART1_MAX_RECV_LEN)	//�����Խ�������
			{
				TIM_SetCounter(TIM2,0);//���������    
				
				if(USART1_RX_STA==0) 				//ʹ�ܶ�ʱ��7���ж� 
				{
					TIM_Cmd(TIM2,ENABLE);//ʹ�ܶ�ʱ��7
				}
				
				USART1_RX_BUF[USART1_RX_STA++]=res;	//��¼���յ���ֵ	 
			}
			else if(USART1_RX_STA>=USART1_MAX_RECV_LEN)
			{
				TIM_SetCounter(TIM2,0);//��������� 
				
				
				
				u16 temp=USART1_RX_STA-USART1_MAX_RECV_LEN;
				USART1_RX_STA++;
				USART1_2_RX_BUF[temp]=res;
				
			}
			else 
			{
				USART1_RX_STA|=1<<15;				//ǿ�Ʊ�ǽ������
			}
		}
	}
}


//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void USART1_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); //����2ʱ��ʹ��

 	USART_DeInit(USART1);  //��λ����2
		 //USART1_TX   PA9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2
   
    //USART1_RX	  PA10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3
	
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
	USART_Init(USART1, &USART_InitStructure); //��ʼ������2
  

	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 
	
	//ʹ�ܽ����ж�
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�   
	
	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	
	TIM2_Int_Init(99,7199);		//10ms�ж�
	USART1_RX_STA=0;		//����
	TIM_Cmd(TIM2,DISABLE);			//�رն�ʱ��7

}

//����2,printf ����
//ȷ��һ�η������ݲ�����USART1_MAX_SEND_LEN�ֽ�
void u1_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART1_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART1_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART1,USART1_TX_BUF[j]); 
	} 
}

 



















