#include "usart.h"	  
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      

	while((USART1->SR&0X40)==0);
	USART1->DR = (u8) ch;      
  return ch;
}
#endif 

int Usart_Receive;
/**************************实现函数**********************************************
*功    能:		usart1发送一个字节
*********************************************************************************/
void usart1_send(u8 data)
{
	USART1->DR = data;
	while((USART1->SR&0x40)==0);	
}
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  
	RCC->APB2ENR|=1<<14;  //使能串口时钟 
	GPIOA->CRH&=0XFFFFF00F;//IO状态设置
	GPIOA->CRH|=0X000008B0;//IO状态设置
		  
	RCC->APB2RSTR|=1<<14;   //复位串口1
	RCC->APB2RSTR&=~(1<<14);//停止复位	   	   
	//波特率设置
 	USART1->BRR=mantissa; // 波特率设置	 
	USART1->CR1|=0X200C;  //1位停止,无校验位.
	USART1->CR1|=1<<8;    //PE中断使能
	USART1->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(0,0,USART1_IRQn,2);//组2，最低优先级 
}
//初始化IO 串口2
//pclk1:PCLK1时钟频率
//bound:波特率
void uart2_init(u32 pclk1,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk1*1000000)/(bound*16);	//得到USARTDIV
	mantissa=temp;						//得到整数部分
	fraction=(temp-mantissa)*16;  //得到小数部分
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<3;   //使能PORTB时钟
	RCC->APB1ENR|=1<<17; 	//使能串口时钟
	GPIOA->CRL&=0XFFFFF00F;	//IO状态设置
	GPIOA->CRL|=0X000008B0;	//IO状态设置
	RCC->APB1RSTR|=1<<17;   //复位串口2
	RCC->APB1RSTR&=~(1<<17);  //停止复位
	//波特率设置
 	USART2->BRR=mantissa;
	USART2->CR1|=0X200C; //1位停止 无校验位 
	USART2->CR1 |= 1 << 8;
	USART2->CR1 |= 1 << 5;
	MY_NVIC_Init(0,0,USART2_IRQn,2);
}
void USART2_IRQHandler(void)
{
	if(USART2->SR&(1<<5))	//接收到数据
	{	 
		if(!set_pwm_flag)
		{
			static unsigned char count = 0;
			Rrxbuf[count] = USART2->DR;
			count++;
			if(count >= 3)
			{
				count = 0;
				if((Rrxbuf[0] == 0x01) && (Rrxbuf[1] == 0xfe))
					set_pwm_flag = 1;
				else
					set_pwm_flag = 0;
			}
		}
	}
}



