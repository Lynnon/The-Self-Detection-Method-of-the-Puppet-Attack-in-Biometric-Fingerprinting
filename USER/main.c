
#include "sys.h"
#include "delay.h"
#include "oled_iic.h"
#include "stdio.h"
#include "key.h"
#include "timer.h"
#include "as608.h"
#include "USART1.h"
#include "USART2.h"

int main(void)
{
	extern const u8 BMP1[];
	HZ= GB16_NUM();
	delay_init();
	OLED_Init();
	u8 i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	USART1_init(57600);
	Usart2_Init(115200);
	KEY_Init();

		OLED_Clear();

		Del_FR();

	while(1)
	{
	
		Add_FR();
		
//				if(KEY2==0)
//		{
//		press_FR();
//		}
//		
//						if(KEY3==0)
//		{
//		Del_FR();
//		}
		
	}
}


