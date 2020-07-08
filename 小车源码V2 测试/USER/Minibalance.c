#include "sys.h"
#include "math.h"

float Encoder_A,Encoder_B,Encoder_C,Encoder_D;          //±àÂëÆ÷µÄÂö³å¼ÆÊý
float Position_A,Position_B,Position_C,Position_D;
               
long int Motor_A,Motor_B,Motor_C,Motor_D;        //µç»úPWM±äÁ¿
float Target_A=0,Target_B=0,Target_C=0,Target_D=0;     //µç»úËÙ¶ÈÄ¿±êÖµ

int Voltage;                             //µç³ØµçÑ¹²ÉÑùÏà¹ØµÄ±äÁ¿  ºóÆÚ×ö
                     
u8 delay_50,delay_flag;                          //ÑÓÊ±Ïà¹Ø±äÁ¿
u8 Urxbuf[17],Utxbuf[21],Usart_ON_Flag=0;  //´®¿Ú¿ØÖÆÏà¹Ø±äÁ¿
float Pitch,Roll,Yaw,Move_X,Move_Y,Move_Z;   //ÈýÖá½Ç¶ÈºÍXYZÖáÄ¿±êËÙ¶È
float yaw_offset;
u16 PID_Parameter[10],Flash_Parameter[10];  //FlashÏà¹ØÊý×é		//Ð´ÈëÎ»ÖÃ µç×ÓÂÞÅÌÏà¹Ø±äÁ¿

float Velocity_KP=4,Velocity_KI=4;	          //ËÙ¶È¿ØÖÆPID²ÎÊý

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
	Stm32_Clock_Init(9);            //=====ÏµÍ³Ê±ÖÓÉèÖÃ
	delay_init(72);                 //=====ÑÓÊ±³õÊ¼»¯
	JTAG_Set(JTAG_SWD_DISABLE);     //=====¹Ø±ÕJTAG½Ó¿Ú
	JTAG_Set(SWD_ENABLE);           //=====´ò¿ªSWD½Ó¿Ú ¿ÉÒÔÀûÓÃÖ÷°åµÄSWD½Ó¿Úµ÷ÊÔ
	LED_Init();
	MiniBalance_PWM_Init(7199,0);   //=====³õÊ¼»¯PWM 10KHZ£¬ÓÃÓÚÇý¶¯µç»ú
	OLED_Init();                    //=====OLED³õÊ¼»¯
	
	RCC->APB2ENR |= 1 << 2;
	GPIOA->CRH &= 0xfff0ffff;
	GPIOA->CRH |= 0x00080000;
	offset_x = (xmax + xmin) / 2;
	offset_y = (ymax + ymin) / 2;
	x_gain = 1;
	y_gain = (xmax - xmin) / (ymax - ymin);
	
	uart_init(72,115200);           //=====´®¿Ú1³õÊ¼»¯
	uart2_init(36,9600);						//½ÓÊÕÒ£¿ØÖ¸Áî
	uart3_init(36,115200);          //=====´®¿Ú3³õÊ¼»¯ ÓëTX2Í¨ÐÅÓÃ
	Encoder_Init_TIM2();            //=====±àÂëÆ÷½Ó¿Ú
	Encoder_Init_TIM3();            //=====±àÂëÆ÷½Ó¿Ú
	Encoder_Init_TIM4();            //=====³õÊ¼»¯±àÂëÆ÷C
	Encoder_Init_TIM5();            //=====³õÊ¼»¯±àÂëÆ÷D
	IIC_Init();                     //=====IIC³õÊ¼»¯
  MPU6050_initialize();           //=====MPU6050³õÊ¼»¯	
	//HMC5883_Init();                 ///////////////////////////////////////////
  DMP_Init();                     //=====³õÊ¼»¯DMP
	EXTI_Init();                    //=====MPU6050 5ms¶¨Ê±ÖÐ¶Ï³õÊ¼»
	//sheng_init();
	//trig_init();
  while(1)
	{	
		if(power_on > 5000)
				oled_show();
		delay_flag=1;	
		delay_50=0;
		while(delay_flag);	       //Í¨¹ýMPU6050µÄINTÖÐ¶ÏÊµÏÖµÄ50ms¾«×¼ÑÓÊ±	
	} 
}







