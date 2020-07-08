#include "sys.h"
#include "chaosheng.h"
#define TRIG PAout(11)
u8 TIM1CH1_CAPTURE_STA = 0;
u16 TIM1CH1_CAPTURE_VAL;

void sheng_init(void)
{
	RCC->APB2ENR |= 1 << 11;								//开启TIM1时钟
	
	RCC->APB2ENR |= 1 << 2;								//开启GPIOA时钟
	GPIOA->CRH &= 0xfffffff0;		
	GPIOA->CRH |= 0x00000008;							//设置PB6为上下拉输入
	GPIOA->ODR |= 0 << 8;									//PB6下拉
	TIM1->ARR = 0xffff;
	TIM1->PSC = 71;												//1us精度
	TIM1->CCMR1 |= 1 << 0;								//CH1配置为IC1
	TIM1->CCMR1 |= 0 << 4;								//配置输入滤波器 不滤波
	TIM1->CCMR1 |= 0 << 2;								//输入预分频器 不分频
	TIM1->CCER |= 0 << 1;									//上升沿捕获
	TIM1->CCER |= 1 << 0;									//允许捕获计数器
	TIM1->DIER |= 1 << 1;									//允许捕获中断
	TIM1->DIER |= 1 << 0;									//允许中断更新
	
	TIM1->CR1 |= 0x01;										//启动定时器4
	MY_NVIC_Init(2,0,TIM1_CC_IRQn,2);				//
}
void TIM1_CC_IRQHandler(void)
{
	u16 tsr;
	tsr = TIM1->SR;
	if((TIM1CH1_CAPTURE_STA & 0x80) == 0)									//还未成功捕获 或正在处理
	{	
		if(tsr & 0x01)				//如果溢出了
		{
			if(TIM1CH1_CAPTURE_STA & 0x40)	//如果已经捕获过高电平
			{
				if((TIM1CH1_CAPTURE_STA & 0x3f) == 0x3f)		//如果已经达到最大计数范围
				{	
					TIM1CH1_CAPTURE_STA |= 0x80;								//标记成功捕获
					TIM1CH1_CAPTURE_VAL = 0xffff;								
				}
				else
					TIM1CH1_CAPTURE_STA++;							//溢出增加
			}
		}
		if(tsr & 0x02)						//如果是跳变沿捕获
		{
			if(TIM1CH1_CAPTURE_STA & 0x40)	//如果已经捕获过高电平
			{
				TIM1CH1_CAPTURE_STA |= 0x80;			//标记成功捕获
				TIM1CH1_CAPTURE_VAL = TIM1->CCR1;	//获取小时间
				TIM1->CCER &= ~(1 << 1);						//设置为上升沿捕获
			}
			else															//如果还没开始捕获
			{
				TIM1CH1_CAPTURE_STA = 0;			//清空
				TIM1CH1_CAPTURE_VAL = 0;			//清空
				TIM1CH1_CAPTURE_STA |= 0x40;		//标记捕获到了上升沿
				TIM1->CNT = 0;							//计数器清空
				TIM1->CCER |= 1 << 1;						//设置为下降沿捕获
			}
		}
	}
	TIM1->SR = 0;														//清除中断标志位
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
