
#include "usartx.h"

/**************************ʵ�ֺ���**********************************************
*��    ��:		usart4����һ���ֽ�
*********************************************************************************/
void usart3_send(u8 data)
{
	USART3->DR = data;
	while((USART3->SR&0x40)==0);	
}
/**************************************************************************
�������ܣ�����3��ʼ��
��ڲ�����pclk2:PCLK2 ʱ��Ƶ��(Mhz)    bound:������
����  ֵ����
**************************************************************************/
void uart3_init(u32 pclk2,u32 bound)
{  	 
float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
  mantissa<<=4;
	mantissa+=fraction; 
	

	RCC->APB2ENR|=1<<0;    //��������ʱ��
	RCC->APB2ENR|=1<<4;   //ʹ��PORTC��ʱ��  
	RCC->APB1ENR|=1<<18;  //ʹ�ܴ���ʱ�� 
	GPIOC->CRH&=0XFFFF00FF; 
	GPIOC->CRH|=0X00008B00;//IO״̬����
	GPIOC->ODR|=1<<10;	 
  AFIO->MAPR|=1<<4;      //������ӳ��

	RCC->APB1RSTR|=1<<18;   //��λ����1
	RCC->APB1RSTR&=~(1<<18);//ֹͣ��λ	   	   
	//����������
 	USART3->BRR=mantissa; // ����������	 
	USART3->CR1|=0X200C;  //1λֹͣ,��У��λ.
	//ʹ�ܽ����ж�
	USART3->CR1|=1<<8;    //PE�ж�ʹ��
	USART3->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(0,0,USART3_IRQn,2);//��2��������ȼ� 
}

/**************************************************************************
�������ܣ�����3�����ж�	���Խ����һ���������޷��������� �����µ���쳣������
��ڲ�������
����  ֵ����
**************************************************************************/
void USART3_IRQHandler(void)	
{	
	if(USART3->SR&(1<<5))//���յ�����
	{	      
				u8 temp;
				static u8 last_data,last_last_data,Usart_ON_Count;
				temp=USART3->DR;			//�������һ������
				last_last_data=last_data;
				last_data=temp;				
				if(Usart_ON_Flag==0)	//�ȴ����������ݲſ�ʼ��һ�ν���
				{	
						Urxbuf[Usart_ON_Count] = temp;
						if(++Usart_ON_Count > 9) 
						{
								Usart_ON_Count = 0;
								if(( '\r' == last_last_data) && ('\n' == last_data))
									Usart_ON_Flag = 1;
						}
						if(( '\r' == last_last_data) && ('\n' == last_data))
							Usart_ON_Count = 0;
				}
   }

}



