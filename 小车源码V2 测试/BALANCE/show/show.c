#include "show.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
unsigned char i;          //计数变量
unsigned char Send_Count; //串口需要发送的数据个数
float Vol;
/**************************************************************************
函数功能：OLED显示
入口参数：无
返回  值：无
**************************************************************************/
void oled_show(void)
{
	//清除显示 不可行
	//OLED_Clear();
	//第一行 显示X坐标
	OLED_ShowString(0,0,"POS_X: ");
	if(POSITION_X < 0)
	{
		OLED_ShowChar(56,0,'-',12,1);
		OLED_ShowNumber(64,0,(u32)(-POSITION_X),4,12);
		OLED_ShowChar(96,0,'.',12,1);  OLED_ShowChar(104,0,'0'+((u32)((-POSITION_X)*10)%10),12,1);  OLED_ShowChar(112,0,'0'+((u32)((-POSITION_X)*100)%10),12,1);
	}
	else
	{
		OLED_ShowChar(56,0,'+',12,1);
		OLED_ShowNumber(64,0,(u32)(POSITION_X),4,12);
		OLED_ShowChar(96,0,'.',12,1);  OLED_ShowChar(104,0,'0'+((u32)((POSITION_X)*10)%10),12,1);  OLED_ShowChar(112,0,'0'+((u32)((POSITION_X)*100)%10),12,1);
	}
	//第二行 显示Y坐标
	OLED_ShowString(0,12,"POS_Y: ");
	if(POSITION_Y < 0)
	{
		OLED_ShowChar(56,12,'-',12,1);
		OLED_ShowNumber(64,12,(u32)(-POSITION_Y),4,12);
		OLED_ShowChar(96,12,'.',12,1); OLED_ShowChar(104,12,'0'+((u32)((-POSITION_Y)*10)%10),12,1); OLED_ShowChar(112,12,'0'+((u32)((-POSITION_Y)*100)%10),12,1);
	}
	else
	{
		OLED_ShowChar(56,12,'+',12,1);
		OLED_ShowNumber(64,12,(u32)(POSITION_Y),4,12);
		OLED_ShowChar(96,12,'.',12,1); OLED_ShowChar(104,12,'0'+((u32)((POSITION_Y)*10)%10),12,1); OLED_ShowChar(112,12,'0'+((u32)((POSITION_Y)*100)%10),12,1);
	}
	//第三行 显示线速度
	OLED_ShowString(0,24,"Vel_L: ");
	if(Vel_Y < 0)
	{
		OLED_ShowChar(56,24,'-',12,1);
		OLED_ShowNumber(64,24,(u32)(-Vel_Y),4,12);
		OLED_ShowChar(96,24,'.',12,1); OLED_ShowChar(104,24,'0'+((u32)((-Vel_Y)*10)%10),12,1);      OLED_ShowChar(112,24,'0'+((u32)((-Vel_Y)*100)%10),12,1);
	}
	else
	{
		OLED_ShowChar(56,24,'+',12,1);
		OLED_ShowNumber(64,24,(u32)(Vel_Y),4,12);
		OLED_ShowChar(96,24,'.',12,1); OLED_ShowChar(104,24,'0'+((u32)((Vel_Y)*10)%10),12,1);      OLED_ShowChar(112,24,'0'+((u32)((Vel_Y)*100)%10),12,1);
	}
	//第四行 显示角速度
	OLED_ShowString(0,36,"Vel_A: ");
	if(Vel_Z < 0)
	{
		OLED_ShowChar(56,36,'-',12,1);
		OLED_ShowNumber(64,36,(u32)(-Vel_Z),4,12);
		OLED_ShowChar(96,36,'.',12,1); OLED_ShowChar(104,36,'0'+((u32)((-Vel_Z)*10)%10),12,1);      OLED_ShowChar(112,36,'0'+((u32)((-Vel_Z)*100)%10),12,1);
	}
	else
	{
		OLED_ShowChar(56,36,'+',12,1);
		OLED_ShowNumber(64,36,(u32)(Vel_Z),4,12);
		OLED_ShowChar(96,36,'.',12,1); OLED_ShowChar(104,36,'0'+((u32)((Vel_Z)*10)%10),12,1);      OLED_ShowChar(112,36,'0'+((u32)((Vel_Z)*100)%10),12,1);
	}
	//第五行 显示偏航角
	OLED_ShowString(0,48,"_YAW: ");
	OLED_ShowNumber(64,48,(u32)(Real_Angle),4,12);
	OLED_ShowChar(96,48,'.',12,1); OLED_ShowChar(104,48,'0'+((u32)(Real_Angle*10)%10),12,1); OLED_ShowChar(112,48,'0'+((u32)(Real_Angle*100)%10),12,1);
	// 刷新显示
	/*OLED_ShowString(0,20,"Yaw:");
	if(Yaw < 0)
	{
		OLED_ShowChar(32,20,'-',12,1);
		OLED_ShowNumber(40,20,(u32)(-Yaw),3,12);
		OLED_ShowChar(64,20,'.',12,1); OLED_ShowChar(72,20,'0'+((u32)(-Yaw*10)%10),12,1); OLED_ShowChar(80,20,'0'+((u32)(-Yaw*100)%10),12,1);
	}
	else
	{
		OLED_ShowChar(32,20,'+',12,1);
		OLED_ShowNumber(40,20,(u32)(Yaw),3,12);
		OLED_ShowChar(64,20,'.',12,1); OLED_ShowChar(72,20,'0'+((u32)(Yaw*10)%10),12,1); OLED_ShowChar(80,20,'0'+((u32)(Yaw*100)%10),12,1);
	}
	OLED_ShowString(0,36,"R_A:");
	if(Real_Angle < 0)
	{
		OLED_ShowChar(32,36,'-',12,1);
		OLED_ShowNumber(40,36,(u32)(-Real_Angle),3,12);
		OLED_ShowChar(64,36,'.',12,1); OLED_ShowChar(72,36,'0'+((u32)(-Real_Angle*10)%10),12,1); OLED_ShowChar(80,36,'0'+((u32)(-Real_Angle*100)%10),12,1);
	}
	else
	{
		OLED_ShowChar(32,36,'+',12,1);
		OLED_ShowNumber(40,36,(u32)(Real_Angle),3,12);
		OLED_ShowChar(64,36,'.',12,1); OLED_ShowChar(72,36,'0'+((u32)(Real_Angle*10)%10),12,1); OLED_ShowChar(80,36,'0'+((u32)(Real_Angle*100)%10),12,1);
	}*/
	OLED_Refresh_Gram();
}



