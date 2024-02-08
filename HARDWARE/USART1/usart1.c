#include "delay.h"
#include "USART1.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "timer.h"
  

//串口接收缓存区 	
u8 USART1_RX_BUF[USART1_MAX_RECV_LEN]; 				//接收缓冲,最大USART1_MAX_RECV_LEN个字节.
u8 USART1_TX_BUF[USART1_MAX_SEND_LEN]; 			  //发送缓冲,最大USART1_MAX_SEND_LEN字节

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 USART1_RX_STA=0;   	

u8 USART1_2_RX_BUF[USART1_MAX_RECV_LEN];


void USART1_IRQHandler(void)
{
	u8 res;	      
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		res =USART_ReceiveData(USART1);		 
		if((USART1_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
		{ 
			if(USART1_RX_STA<USART1_MAX_RECV_LEN)	//还可以接收数据
			{
				TIM_SetCounter(TIM2,0);//计数器清空    
				
				if(USART1_RX_STA==0) 				//使能定时器7的中断 
				{
					TIM_Cmd(TIM2,ENABLE);//使能定时器7
				}
				
				USART1_RX_BUF[USART1_RX_STA++]=res;	//记录接收到的值	 
			}
			else if(USART1_RX_STA>=USART1_MAX_RECV_LEN)
			{
				TIM_SetCounter(TIM2,0);//计数器清空 
				
				
				
				u16 temp=USART1_RX_STA-USART1_MAX_RECV_LEN;
				USART1_RX_STA++;
				USART1_2_RX_BUF[temp]=res;
				
			}
			else 
			{
				USART1_RX_STA|=1<<15;				//强制标记接收完成
			}
		}
	}
}


//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void USART1_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); //串口2时钟使能

 	USART_DeInit(USART1);  //复位串口2
		 //USART1_TX   PA9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2
   
    //USART1_RX	  PA10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3
	
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  
	USART_Init(USART1, &USART_InitStructure); //初始化串口2
  

	USART_Cmd(USART1, ENABLE);                    //使能串口 
	
	//使能接收中断
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断   
	
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	
	TIM2_Int_Init(99,7199);		//10ms中断
	USART1_RX_STA=0;		//清零
	TIM_Cmd(TIM2,DISABLE);			//关闭定时器7

}

//串口2,printf 函数
//确保一次发送数据不超过USART1_MAX_SEND_LEN字节
void u1_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART1_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART1_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART1,USART1_TX_BUF[j]); 
	} 
}

 



















