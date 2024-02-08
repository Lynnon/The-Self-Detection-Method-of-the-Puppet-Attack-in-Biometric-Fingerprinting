#include "USART2.h"

#if  USART2_RX_ENABLE                   //���ʹ�ܽ��չ���
char Usart2_RxCompleted = 0;            //����һ������ 0����ʾ����δ��� 1����ʾ������� 
unsigned int Usart2_RxCounter = 0;      //����һ����������¼����2�ܹ������˶����ֽڵ�����
char Usart2_RxBuff[USART2_RXBUFF_SIZE]; //����һ�����飬���ڱ��洮��2���յ�������   	
#endif

void Usart2_Init(unsigned int bound2)
{
			 GPIO_InitTypeDef GPIO_InitStructure;     //����һ������GPIO���ܵı���
			 USART_InitTypeDef USART_InitStructure;   //����һ�����ô��ڹ��ܵı���
#if USART2_RX_ENABLE                         //���ʹ�ܽ��չ���
			 NVIC_InitTypeDef NVIC_InitStructure;     //���ʹ�ܽ��չ��ܣ�����һ�������жϵı���
#endif
	
#if USART2_RX_ENABLE                                 //���ʹ�ܽ��չ���
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�����ж��������飺��2�� �������ȼ���0 1 2 3 �����ȼ���0 1 2 3
#endif	
	
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);  //ʹ�ܴ���2ʱ��
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);   //ʹ��GPIOAʱ��
			USART_DeInit(USART2);                                  //����2�Ĵ�����������ΪĬ��ֵ
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;              //׼������PA2
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //IO����50M
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	       //����������������ڴ���2�ķ���
			GPIO_Init(GPIOA, &GPIO_InitStructure);                 //����PA2
			 
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;              //׼������PA3
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //�������룬���ڴ���2�Ľ���
			GPIO_Init(GPIOA, &GPIO_InitStructure);                 //����PA3
			
			USART_InitStructure.USART_BaudRate = bound2;                                    //����������
			USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //8������λ
			USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //1��ֹͣλ
			USART_InitStructure.USART_Parity = USART_Parity_No;                            //����żУ��λ
			USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������
	
#if USART2_RX_ENABLE               												   //���ʹ�ܽ���ģʽ
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	               //�շ�ģʽ
#else                                                                              //�����ʹ�ܽ���ģʽ
	USART_InitStructure.USART_Mode = USART_Mode_Tx ;	                           //ֻ��ģʽ
#endif        
    USART_Init(USART2, &USART_InitStructure);                                      //���ô���

#if USART2_RX_ENABLE  	         					        //���ʹ�ܽ���ģʽ
	USART_ClearFlag(USART2, USART_FLAG_RXNE);	            //������ձ�־λ
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);          //���������ж�
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;       //���ô���2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0; //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//�ж�ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                        //���ô���2�ж�
#endif  

	USART_Cmd(USART2, ENABLE);                              //ʹ�ܴ���2
}
/*-------------------------------------------------*/
/*������������2 printf����                         */
/*��  ����char* fmt,...  ��ʽ������ַ����Ͳ���    */
/*����ֵ����                                       */
/*-------------------------------------------------*/

__align(8) char USART2_TxBuff[USART2_TXBUFF_SIZE];  

void u2_printf(char* fmt,...) 
{  
	unsigned int i,length;
	
	va_list ap;
	va_start(ap,fmt);
	vsprintf(USART2_TxBuff,fmt,ap);
	va_end(ap);	
	
	length=strlen((const char*)USART2_TxBuff);		
	while((USART2->SR&0X40)==0);
	for(i = 0;i < length;i ++)
	{			
		USART2->DR = USART2_TxBuff[i];
		while((USART2->SR&0X40)==0);	
	}	
}

/*-------------------------------------------------*/
/*������������2�����жϺ���                        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void USART2_IRQHandler(void)   
{  
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET) //USART_IT_TC����Ҳ���滻
			{    
				USART_ClearITPendingBit(USART2, USART_IT_TXE);
			}
			
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){  //���USART_IT_RXNE��־��λ����ʾ�����ݵ��ˣ�����if��֧
		if(USART2->DR)
		{                                     //����ֵ�ű��浽������	
			Usart2_RxBuff[Usart2_RxCounter]=USART2->DR;     //���浽������	
			Usart2_RxCounter ++;                            //ÿ����1���ֽڵ����ݣ�Usart2_RxCounter��1����ʾ���յ���������+1 
		}		
	}
} 


