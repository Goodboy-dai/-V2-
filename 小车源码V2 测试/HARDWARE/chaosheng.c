#include "sys.h"
#include "chaosheng.h"
#define TRIG PAout(11)
u8 TIM1CH1_CAPTURE_STA = 0;
u16 TIM1CH1_CAPTURE_VAL;

void sheng_init(void)
{
	RCC->APB2ENR |= 1 << 11;								//����TIM1ʱ��
	
	RCC->APB2ENR |= 1 << 2;								//����GPIOAʱ��
	GPIOA->CRH &= 0xfffffff0;		
	GPIOA->CRH |= 0x00000008;							//����PB6Ϊ����������
	GPIOA->ODR |= 0 << 8;									//PB6����
	TIM1->ARR = 0xffff;
	TIM1->PSC = 71;												//1us����
	TIM1->CCMR1 |= 1 << 0;								//CH1����ΪIC1
	TIM1->CCMR1 |= 0 << 4;								//���������˲��� ���˲�
	TIM1->CCMR1 |= 0 << 2;								//����Ԥ��Ƶ�� ����Ƶ
	TIM1->CCER |= 0 << 1;									//�����ز���
	TIM1->CCER |= 1 << 0;									//�����������
	TIM1->DIER |= 1 << 1;									//�������ж�
	TIM1->DIER |= 1 << 0;									//�����жϸ���
	
	TIM1->CR1 |= 0x01;										//������ʱ��4
	MY_NVIC_Init(2,0,TIM1_CC_IRQn,2);				//
}
void TIM1_CC_IRQHandler(void)
{
	u16 tsr;
	tsr = TIM1->SR;
	if((TIM1CH1_CAPTURE_STA & 0x80) == 0)									//��δ�ɹ����� �����ڴ���
	{	
		if(tsr & 0x01)				//��������
		{
			if(TIM1CH1_CAPTURE_STA & 0x40)	//����Ѿ�������ߵ�ƽ
			{
				if((TIM1CH1_CAPTURE_STA & 0x3f) == 0x3f)		//����Ѿ��ﵽ��������Χ
				{	
					TIM1CH1_CAPTURE_STA |= 0x80;								//��ǳɹ�����
					TIM1CH1_CAPTURE_VAL = 0xffff;								
				}
				else
					TIM1CH1_CAPTURE_STA++;							//�������
			}
		}
		if(tsr & 0x02)						//����������ز���
		{
			if(TIM1CH1_CAPTURE_STA & 0x40)	//����Ѿ�������ߵ�ƽ
			{
				TIM1CH1_CAPTURE_STA |= 0x80;			//��ǳɹ�����
				TIM1CH1_CAPTURE_VAL = TIM1->CCR1;	//��ȡСʱ��
				TIM1->CCER &= ~(1 << 1);						//����Ϊ�����ز���
			}
			else															//�����û��ʼ����
			{
				TIM1CH1_CAPTURE_STA = 0;			//���
				TIM1CH1_CAPTURE_VAL = 0;			//���
				TIM1CH1_CAPTURE_STA |= 0x40;		//��ǲ�����������
				TIM1->CNT = 0;							//���������
				TIM1->CCER |= 1 << 1;						//����Ϊ�½��ز���
			}
		}
	}
	TIM1->SR = 0;														//����жϱ�־λ
}

void trig_init(void)
{
	RCC->APB2ENR |= 1 << 2;
	GPIOA->CRH &= 0xffff0fff;
	GPIOA->CRH |= 0x00003000;
	TRIG = 0;
}
void trig(void)
{
	TRIG = 1;
	delay_us(20);
	TRIG = 0;
}
void get_Distance(void)
{
	u32 temp = 0;
	if(TIM1CH1_CAPTURE_STA & 0x80)
	{
			temp = TIM1CH1_CAPTURE_STA & 0x3f;
			temp *= 65536;
			temp += TIM1CH1_CAPTURE_VAL;
			Real_Distance = (double)temp*170/10000;
			TIM1CH1_CAPTURE_STA = 0;
	}
}
