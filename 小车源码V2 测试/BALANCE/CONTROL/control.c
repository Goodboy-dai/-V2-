#include "control.h"	
#include "filter.h"	
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
u8 Flag_Target,Flag_Change;                             //相关标志位
u8 temp1;                                               //临时变量
float Voltage_Count,Voltage_All;  //电压采样相关变量
float Gyro_K=0.004;       //陀螺仪比例系数
int j;
///a_PARAMETER为横向两轮间距的一半 b为纵向的一半
#define a_PARAMETER          (311.0f)               
#define b_PARAMETER          (307.5f)         
/**************************************************************************
函数功能：小车运动数学模型
入口参数：X Y Z 三轴速度或者位置
返回  值：无
**************************************************************************/
void Kinematic_Analysis(float Vx,float Vy,float Vz)
{
		Target_A   = -Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
		Target_B   = +Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
		Target_C   = -Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
		Target_D   = +Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
}
/**************************************************************************
函数功能：所有的控制代码都在这里面
         5ms定时中断由MPU6050的INT引脚触发
         严格保证采样和数据处理的时间同步				 
**************************************************************************/
int EXTI15_10_IRQHandler(void) 
{    
	static int send;
	 if(INT==0)		
	{     
		  EXTI->PR=1<<15;    
			Flag_Target=!Flag_Target;		
			if(delay_flag==1)
			 {
				 if(++delay_50==10)	 delay_50=0,delay_flag=0;                     //给主函数提供50ms的精准延时
			 }
			if(Flag_Target==1)   
			{				//5ms读取一次陀螺仪和加速度计
#if 1
					Read_DMP();                                                           //===更新姿态			
#endif
					return 0;	                                               
			}
#if 1
					Read_DMP();          //===更新姿态	                       
#endif					   
  		Led_Flash(100);              			//===LED闪烁;常规模式 1s改变一次指示灯的状态	
			
		//	trig();
		//	get_Distance();
			
			Encoder_A=Read_Encoder(2)*1.3265; 	//单位为mm /s
			Position_A += Encoder_A;			
			Encoder_B=Read_Encoder(3)*1.3265; 
			Position_B += Encoder_B;
			Encoder_C=-Read_Encoder(4)*1.3265; 
			Position_C += Encoder_C;
			Encoder_D=-Read_Encoder(5)*1.3265; 
			Position_D += Encoder_D;
			
			//发送里程信息 （后期与电子罗盘用卡尔曼滤波融合为angle角:现阶段不可行，校园内磁场环境复杂，自行车多，用slam或vslam的yaw融合）
			//Vel_X = (float)(Encoder_B - Encoder_A) / 2;			//单位为1mm / s
			Vel_Z = (float)(Encoder_D - Encoder_B) / 2 / 1000 * 100;		//单位为弧度每秒
			Vel_Y = (float)(Encoder_B + Encoder_C) / 2;			//单位为1mm / s
			//Real_Angle += -Vel_Z / 100;
			
			
			if(power_on < 3200)
			{
				power_on++;
				if(power_on > 3000)
				{
					yaw_offset = Yaw;
					power_on = 6000;
				}
			}
			else
			{
				Real_Angle = Yaw - yaw_offset;
				if(Real_Angle < 0)
				{
					Real_Angle += 360;
				}
				angle = ((double)Real_Angle / 180) * 3.1415926535;
				POSITION_X += +Vel_Y / 100 * sin(angle);
				POSITION_Y += +Vel_Y / 100 * cos(angle);
			}
			
#if !DEBUG
			/*if(power_on < 250)					//初始零点调校 (电子罗盘版本)
			{
				power_on++;
				if((power_on > 100) && (power_on < 200))
				{
					Read_5883(HMC5883_Addr,0x03,6,BUF);
					X = ((u16)BUF[0] << 8) | BUF[1];
					Y = ((u16)BUF[4] << 8) | BUF[5];
					angle = (atan2((double)(X-offset_x),(double)(Y-offset_y)) * (180 / 3.14159265) + 180);
					angle_sum += angle;
					angle_count++;
				}
				else if(power_on > 200)
				{
					light_offset = angle_sum / angle_count;
					power_on = 300;
				}
			}
			else									//正常工作状态
			{
				Read_5883(HMC5883_Addr,0x03,6,BUF);
				X = ((u16)BUF[0] << 8) | BUF[1];
				Y = ((u16)BUF[4] << 8) | BUF[5];
				angle = (atan2((double)(X-offset_x),(double)(Y-offset_y)) * (180 / 3.14159265) + 180) - light_offset;
				if(angle < 0)
					angle += 360;
				Real_Angle = Yaw = angle * 3.1415926 / 180;
				POSITION_X += +Vel_Y / 100 *  sin(Yaw);
				POSITION_Y += +Vel_Y / 100 * cos(Yaw);
			}*/
#else
			/////////////////调试 变化磁场即更换作用场所的时候用
			Read_5883(HMC5883_Addr,0x03,6,BUF);
		  X=((u16)BUF[0]<<8)|BUF[1];  
		  Y=((u16)BUF[4]<<8)|BUF[5];  
		  if(X > xmax)
				xmax = X;
		  if(X < xmin)
				xmin = X;
		  if(Y > ymax)
				ymax = Y;
		  if(Y < ymin)
				ymin = Y;
			/////////////////
			
			if(++send > 10)
			{
				//printf("%g\n",angle);
				//printf("Vel_X : %.2f Vel_Y: %.2f Vel_Z: %.2f\n",Vel_X,Vel_Y,Vel_Z );
				//printf("angle: %.2f POSITION_X:%.2f POSITION_Y: %.2f\n",angle,POSITION_X,POSITION_Y);
				//printf("%f\n",xmax);
				send = 0;
			}
#endif
			if(power_on > 5000)
			{
				send_odoemtry();
			}
			////////////
			if((EN)&&(power_on > 5000))
			{
				if(set_pwm_flag)
				{
					handle_u2();
					set_pwm_flag = 0;
				}
				if(Usart_ON_Flag)
				{
					handle_u3();
					//send_sta();
					Usart_ON_Flag = 0;
				}
				/*Target_A = Target_B = 200;
				Target_C = Target_D = 200;*/
			}
			else
			{
				Target_A = Target_B = Target_C = Target_D = 0;
			}
			
			if(PAUSE)
			{
				Target_A = Target_B = Target_C = Target_D = 0;
				//A_Stop = B_Stop = C_Stop = D_Stop = 1;			
			}
			
				Motor_A=Incremental_PI_A((int)(Encoder_A/10),(int)(Target_A/10));                         //===速度闭环控制计算电机A最终PWM
				Motor_B=Incremental_PI_B((int)(Encoder_B/10),(int)(Target_B/10));                         //===速度闭环控制计算电机B最终PWM
				Motor_C=Incremental_PI_C((int)(Encoder_C/10),(int)(Target_C/10));                         //===速度闭环控制计算电机C最终PWM
				Motor_D=Incremental_PI_D((int)(Encoder_D/10),(int)(Target_D/10));   											//
			Xianfu_Pwm(6900);                     //===PWM限幅
		 Set_Pwm(Motor_B,Motor_A,Motor_D,Motor_C);     //===赋值给PWM寄存器  
 }
	 return 0;	 
} 
void handle_u2(void)
{
	PAUSE = 0;
	switch(Rrxbuf[2])
	{
		case 0x05:
			Target_A = 40;
			Target_B = 40;
			Target_C = 40;
			Target_D = 40;
		break;
		case 0x04:
			Target_A = -40;
			Target_B = -40;
			Target_C = -40;
			Target_D = -40;
		break;
		case 0x02:
			Target_A = 40;
			Target_B = -40;
			Target_C = 40;
			Target_D = -40;
		break;
		case 0x0A:
			Target_A = -40;
			Target_B = 40;
			Target_C = -40;
			Target_D = 40;
		break;
		case 0x03:
			Target_A = -40;
			Target_B = -40;
			Target_C = 40;
			Target_D = 40;
		break;
		case 0x0B:
			Target_A = 40;
			Target_B = 40;
			Target_C = -40;
			Target_D = -40;
		break;
		default:
			PAUSE = 1;
			Target_A = 0;
			Target_B = 0;
			Target_C = 0;
			Target_D = 0;
		break;
	}
}


/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int motor_a,int motor_b,int motor_c,int motor_d)
{
   	if(motor_a<0)			INA2=1,			INA1=0;
			else 	          INA2=0,			INA1=1;
		PWMA=myabs(motor_a);
	
		if(motor_b<0)			INB2=1,			INB1=0;
		else 	            INB2=0,			INB1=1;
		PWMB=myabs(motor_b);
	
		if(motor_c>0)			INC2=1,			INC1=0;
		else 	            INC2=0,			INC1=1;
		PWMC=myabs(motor_c);
	
		if(motor_d>0)			IND2=1,			IND1=0;
		else 	            IND2=0,			IND1=1;
		PWMD=myabs(motor_d);
}

/**************************************************************************
函数功能：限制PWM赋值 
入口参数：幅值
返回  值：无
**************************************************************************/
void Xianfu_Pwm(int amplitude)
{	
    if(Motor_A<-amplitude) Motor_A=-amplitude;	
		if(Motor_A>amplitude)  Motor_A=amplitude;	
	  if(Motor_B<-amplitude) Motor_B=-amplitude;	
		if(Motor_B>amplitude)  Motor_B=amplitude;		
	  if(Motor_C<-amplitude) Motor_C=-amplitude;	
		if(Motor_C>amplitude)  Motor_C=amplitude;		
	  if(Motor_D<-amplitude) Motor_D=-amplitude;	
	  if(Motor_D>amplitude)  Motor_D=amplitude;		
}
/**************************************************************************
函数功能：绝对值函数
入口参数：long int
返回  值：unsigned int
**************************************************************************/
u32 myabs(long int a)
{ 		   
	  u32 temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}
/**************************************************************************
函数功能：增量PI控制器
入口参数：编码器测量值，目标速度
返回  值：电机PWM
根据增量式离散PID公式 
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  以此类推 
pwm代表增量输出
在我们的速度控制闭环系统里面，只使用PI控制
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)
**************************************************************************/
int Incremental_PI_A (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 if(Pwm>7200)Pwm=7200;
	 if(Pwm<-7200)Pwm=-7200;
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}
int Incremental_PI_B (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 if(Pwm>7200)Pwm=7200;
	 if(Pwm<-7200)Pwm=-7200;
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}
int Incremental_PI_C (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                                  //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 if(Pwm>7200)Pwm=7200;
	 if(Pwm<-7200)Pwm=-7200;
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}
int Incremental_PI_D (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                                  //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 if(Pwm>7200)Pwm=7200;
	 if(Pwm<-7200)Pwm=-7200;
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}
void send_sta(void)
{
	float *speed_A = NULL,*speed_B = NULL,*speed_C = NULL,*speed_D = NULL;
	speed_A = (float *)(Urxbuf + 0);
	speed_B = (float *)(Urxbuf + 4);
	speed_C = (float *)(Urxbuf + 8);
	speed_D = (float *)(Urxbuf + 12);
	printf("%f %f %f %f \n",*speed_A,*speed_B,*speed_C,*speed_D);
}
void handle_u3(void)
{
	float *speed_left = NULL,*speed_right = NULL;
	speed_right = (float *)(Urxbuf + 0);
	speed_left  = (float *)(Urxbuf + 4);
	Kinematic_Analysis(0,0.5f*((*speed_right)+(*speed_left)),((*speed_right)-(*speed_left))/(2*a_PARAMETER));
}
void send_odoemtry(void)
{
	int i;
	memcpy(Utxbuf,&POSITION_X,4);
	memcpy(Utxbuf + 4,&POSITION_Y,4);
	memcpy(Utxbuf + 8,&angle,4);
	memcpy(Utxbuf + 12,&Vel_Y,4);
	memcpy(Utxbuf + 16,&Vel_Z,4);
	Utxbuf[20] = '\n';
	for (i = 0;i < 21;i++)
	{
		while((USART3->SR&0X40)==0);
		USART3->DR = (u8)Utxbuf[i];
	}
}

