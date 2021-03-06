#include "sys.h"
#include "math.h"

float Encoder_A,Encoder_B,Encoder_C,Encoder_D;          //编码器的脉冲计数
float Position_A,Position_B,Position_C,Position_D;
               
long int Motor_A,Motor_B,Motor_C,Motor_D;        //电机PWM变量
float Target_A=0,Target_B=0,Target_C=0,Target_D=0;     //电机速度目标值

int Voltage;                             //电池电压采样相关的变量  后期做
                     
u8 delay_50,delay_flag;                          //延时相关变量
u8 Urxbuf[17],Utxbuf[21],Usart_ON_Flag=0;  //串口控制相关变量
float Pitch,Roll,Yaw,Move_X,Move_Y,Move_Z;   //三轴角度和XYZ轴目标速度
float yaw_offset;
u16 PID_Parameter[10],Flash_Parameter[10];  //Flash相关数组		//写入位置 电子罗盘相关变量

float Velocity_KP=4,Velocity_KI=4;	          //速度控制PID参数

int Gryo_Z;

int set_pwm_flag;
unsigned char Rrxbuf[3];
float Real_Distance;

float Real_Angle;
float POSITION_X,POSITION_Y;
float Vel_X,Vel_Y,Vel_Z;


unsigned char BUF[6];
short X, Y;
float angle;
double x, y, xmax=-1000, xmin=+1000, ymax=-1000, ymin=+1000,offset_x, offset_y,x_gain,y_gain;
float light_offset, angle_sum;
int power_on = 0, angle_count = 0;
unsigned char PAUSE;

int main(void)
{ 
	Stm32_Clock_Init(9);            //=====系统时钟设置
	delay_init(72);                 //=====延时初始化
	JTAG_Set(JTAG_SWD_DISABLE);     //=====关闭JTAG接口
	JTAG_Set(SWD_ENABLE);           //=====打开SWD接口 可以利用主板的SWD接口调试
	LED_Init();
	MiniBalance_PWM_Init(7199,0);   //=====初始化PWM 10KHZ，用于驱动电机
	OLED_Init();                    //=====OLED初始化
	
	RCC->APB2ENR |= 1 << 2;
	GPIOA->CRH &= 0xfff0ffff;
	GPIOA->CRH |= 0x00080000;
	offset_x = (xmax + xmin) / 2;
	offset_y = (ymax + ymin) / 2;
	x_gain = 1;
	y_gain = (xmax - xmin) / (ymax - ymin);
	
	uart_init(72,115200);           //=====串口1初始化
	uart2_init(36,9600);						//接收遥控指令
	uart3_init(36,115200);          //=====串口3初始化 与TX2通信用
	Encoder_Init_TIM2();            //=====编码器接口
	Encoder_Init_TIM3();            //=====编码器接口
	Encoder_Init_TIM4();            //=====初始化编码器C
	Encoder_Init_TIM5();            //=====初始化编码器D
	IIC_Init();                     //=====IIC初始化
  MPU6050_initialize();           //=====MPU6050初始化	
	//HMC5883_Init();                 ///////////////////////////////////////////
  DMP_Init();                     //=====初始化DMP
	EXTI_Init();                    //=====MPU6050 5ms定时中断初始�
	//sheng_init();
	//trig_init();
  while(1)
	{	
		if(power_on > 5000)
				oled_show();
		delay_flag=1;	
		delay_50=0;
		while(delay_flag);	       //通过MPU6050的INT中断实现的50ms精准延时	
	} 
}







