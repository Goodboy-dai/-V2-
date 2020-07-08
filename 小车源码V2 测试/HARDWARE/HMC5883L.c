#include "sys.h"

void iic_Start(void)
{
	sda_out();
	iic_sda = 1;
	iic_scl = 1;
	delay_us(2);
	iic_sda = 0;
	delay_us(2);
	iic_scl = 0;				//钳住总线 变成主机
}
void iic_Stop(void)
{
	sda_out();
	iic_scl = 0;
	iic_sda = 0;
	delay_us(2);
	iic_scl = 1;							//释放总线
	iic_sda = 1;					
	delay_us(2);
}
void iic_Ack(void)
{
	iic_scl = 0;
	sda_out();
	iic_sda = 0;
	delay_us(2);
	iic_scl = 1;
	delay_us(2);
	iic_scl = 0;
}
void iic_NAck(void)
{
	iic_scl = 0;
	sda_out();
	iic_sda = 1;
	delay_us(2);
	iic_scl = 1;
	delay_us(2);
	iic_scl = 0;
}
unsigned char iic_Wait_Ack(void)
{
	unsigned char Time = 0;
	sda_in()
	iic_sda = 1;
	delay_us(2);
	iic_scl = 1;
	delay_us(2);
	
	while(read_sda)
	{
		Time++;
		if(Time > 250)
		{
			iic_Stop();
			return 1;
		}
	}
	iic_scl = 0;
	return 0;
}
void iic_Send_Byte(unsigned char txd)
{
	unsigned char t;
	sda_out();
	iic_scl = 0;
	for(t = 0;t < 8;t++)
	{
		iic_sda = (txd & 0x80) >> 7;
		txd <<= 1;
		//delay_us(2);
		iic_scl = 1;
		delay_us(2);
		iic_scl = 0;
		delay_us(2);
	}
}
unsigned char iic_Read_Byte(unsigned char ack)
{
	unsigned char i, receive = 0;
	sda_in();
	for(i = 0;i < 8;i++)
	{
		iic_scl = 0;
		delay_us(2);
		iic_scl = 1;
		receive <<= 1;
		if(read_sda) receive++;
		delay_us(2);
	}
	if(!ack)
		iic_NAck();
	else
		iic_Ack();
	return receive;
}
void HMC5883_Init(void)
{
	RCC->APB2ENR |= 1 << 2;
	GPIOA->CRL &= 0xffff00ff;
	GPIOA->CRL |= 0x00003300;
	delay_ms(4);
	Single_Write_5883(0x02,0x00);
}
unsigned char Single_Write_5883(unsigned char reg, unsigned char dat)
{
	iic_Start();
	iic_Send_Byte((HMC5883_Addr << 1) | 0);			//发送器件地址+写命令
	if(iic_Wait_Ack())
	{
		printf("5883 ERROR\n");
		iic_Stop();
		return 1;
	}
	iic_Send_Byte(reg);	//写寄存器地址
	iic_Wait_Ack();			//等待应答
	iic_Send_Byte(dat);//发送数据
	if(iic_Wait_Ack())	//等待ACK
	{
		printf("5883 ERROR\n");
		iic_Stop();
		return 1;
	}
	iic_Stop();
	return 0;
}
unsigned char Single_Read_5883(unsigned reg)
{
	unsigned char res;
	iic_Start();
	iic_Send_Byte((HMC5883_Addr << 1) | 0);	//发送器件地址+写命令
	iic_Wait_Ack();
	iic_Send_Byte(reg);	//写寄存器地址
	iic_Wait_Ack();
	iic_Start();
	iic_Send_Byte((HMC5883_Addr << 1) | 1);	//发送器件地址+读命令
	iic_Wait_Ack();
	res = iic_Read_Byte(0);
	iic_Stop();
	return res;
}
unsigned char Read_5883(unsigned char addr, unsigned char reg, unsigned char len, unsigned char *buf)
{
	iic_Start();
	iic_Send_Byte((addr << 1) | 0);
	if(iic_Wait_Ack())
	{
		iic_Stop();
		return 1;
	}
	iic_Send_Byte(reg);
	iic_Wait_Ack();
	iic_Start();
	iic_Send_Byte((addr << 1) | 1);
	iic_Wait_Ack();
	while(len)
	{
		if(len == 1) *buf=iic_Read_Byte(0);
		else	*buf=iic_Read_Byte(1);
		len--;
		buf++;
	}
	iic_Stop();
	return 0;
}



