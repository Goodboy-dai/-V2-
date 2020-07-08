#include "sys.h"
#include "math.h"

float Encoder_A,Encoder_B,Encoder_C,Encoder_D;          //���������������
float Position_A,Position_B,Position_C,Position_D;
               
long int Motor_A,Motor_B,Motor_C,Motor_D;        //���PWM����
float Target_A=0,Target_B=0,Target_C=0,Target_D=0;     //����ٶ�Ŀ��ֵ

int Voltage;                             //��ص�ѹ������صı���  ������
                     
u8 delay_50,delay_flag;                          //��ʱ��ر���
u8 Urxbuf[17],Utxbuf[21],Usart_ON_Flag=0;  //���ڿ�����ر���
float Pitch,Roll,Yaw,Move_X,Move_Y,Move_Z;   //����ǶȺ�XYZ��Ŀ���ٶ�
float yaw_offset;
u16 PID_Parameter[10],Flash_Parameter[10];  //Flash�������		//д��λ�� ����������ر���

float Velocity_KP=4,Velocity_KI=4;	          //�ٶȿ���PID����

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
	Stm32_Clock_Init(9);            //=====ϵͳʱ������
	delay_init(72);                 //=====��ʱ��ʼ��
	JTAG_Set(JTAG_SWD_DISABLE);     //=====�ر�JTAG�ӿ�
	JTAG_Set(SWD_ENABLE);           //=====��SWD�ӿ� �������������SWD�ӿڵ���
	LED_Init();
	MiniBalance_PWM_Init(7199,0);   //=====��ʼ��PWM 10KHZ�������������
	OLED_Init();                    //=====OLED��ʼ��
	
	RCC->APB2ENR |= 1 << 2;
	GPIOA->CRH &= 0xfff0ffff;
	GPIOA->CRH |= 0x00080000;
	offset_x = (xmax + xmin) / 2;
	offset_y = (ymax + ymin) / 2;
	x_gain = 1;
	y_gain = (xmax - xmin) / (ymax - ymin);
	
	uart_init(72,115200);           //=====����1��ʼ��
	uart2_init(36,9600);						//����ң��ָ��
	uart3_init(36,115200);          //=====����3��ʼ�� ��TX2ͨ����
	Encoder_Init_TIM2();            //=====�������ӿ�
	Encoder_Init_TIM3();            //=====�������ӿ�
	Encoder_Init_TIM4();            //=====��ʼ��������C
	Encoder_Init_TIM5();            //=====��ʼ��������D
	IIC_Init();                     //=====IIC��ʼ��
  MPU6050_initialize();           //=====MPU6050��ʼ��	
	//HMC5883_Init();                 ///////////////////////////////////////////
  DMP_Init();                     //=====��ʼ��DMP
	EXTI_Init();                    //=====MPU6050 5ms��ʱ�жϳ�ʼ�
	//sheng_init();
	//trig_init();
  while(1)
	{	
		if(power_on > 5000)
				oled_show();
		delay_flag=1;	
		delay_50=0;
		while(delay_flag);	       //ͨ��MPU6050��INT�ж�ʵ�ֵ�50ms��׼��ʱ	
	} 
}







