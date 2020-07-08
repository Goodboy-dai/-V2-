#include "map.h"
#include "sys.h"


void Draw_Point(unsigned char x, unsigned char y)
{
	unsigned char pos, bx, temp = 0;
	if (x > MAX_X || y > MAX_Y)return;//超出范围了.
	pos = y / 8;				//获取y坐标对应的二维数组的行值
	bx = y % 8;					//获取在字节中的位置
	temp = 0x01 << bx;			//写入字节
	map[x][pos] |= temp;		
}
unsigned char Read_Point(unsigned char x, unsigned char y)
{
	unsigned char pos, bx, temp = 0;
	if (x > MAX_X || y > MAX_Y)return 1;//超出范围了.
	pos = y / 8;				//获取y坐标对应的二维数组的行值
	bx = y % 8;					//获取在字节中的位置
	temp = map[x][pos];			//获取整个字节
	return ((temp >> bx ) & 0x01);
}
void Draw_Map(unsigned char *point_x,unsigned char *point_y,unsigned char length)
{
	int i;
	for (i = 0; i < length; i++)
		Draw_Point(point_x[i],point_y[i]);
	return;
}

void show_map(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(map[n][i],OLED_DATA); 
	} 
}
void Clear_Point(unsigned char x, unsigned char y)
{
	unsigned char pos, bx, temp = 0;
	if (x > MAX_X || y > MAX_Y)return;//超出范围了.
	pos = y / 8;				//获取y坐标对应的二维数组的行值
	bx = y % 8;					//获取在字节中的位置
	temp = 0x01 << bx;			//写入字节
	map[x][pos] &= ~temp;
}
void map_handle(void)
{
	//先判断是直行还是转弯 即mode 为2 还是 3
	switch(Direction)
	{
		case NORTH:
			if(Read_Point(Start_X,Start_Y + 1))
					mode = 2;
			else
			{
					mode = 3;
			}
			break;
		case NORTHEAST:
			if(Read_Point(Start_X + 1,Start_Y + 1))
					mode = 2;
			else
			{
					mode = 3;
			}
			break;
		case EAST:
			if(Read_Point(Start_X + 1,Start_Y))
					mode = 2;
			else
			{
					mode = 3;
			}
			
			break;
		case SOUTHEAST:
			if(Read_Point(Start_X + 1,Start_Y - 1))
					mode = 2;
			else
			{
					mode = 3;
			}
			break;
		case SOUTH:
			if(Read_Point(Start_X,Start_Y - 1))
					mode = 2;
			else
			{
					mode = 3;
			}
			break;
		case SOUTHWEST:
			if(Read_Point(Start_X - 1,Start_Y - 1))
					mode = 2;
			else
					mode = 3;
			break;
		case WEST:
			if(Read_Point(Start_X - 1,Start_Y))
					mode = 2;
			else
			{
					mode = 3;
			}
			break;
		case NORTHWEST:
			if(Read_Point(Start_X - 1,Start_Y + 1))
					mode = 2;
			else
			{
					mode = 3;
			}
			break;
	}
	
	
	//根据mode 找出要行驶的距离或者要转的角度
	if(2 == mode)
	{
		int i = 1;
		if(NORTH == Direction)
				while(Read_Point(Start_X,Start_Y + i)) 		 Clear_Point(Start_X,Start_Y + i),      Target_X = Start_X,      Target_Y = Start_Y + i, ++i;		//找到局部终点坐标
		
		else if(NORTHEAST == Direction)
				while(Read_Point(Start_X + i,Start_Y + i)) Clear_Point(Start_X + i,Start_Y + i),  Target_X = Start_X + i, Target_Y = Start_Y + i,  ++i;
		
		else if(EAST == Direction)
				while(Read_Point(Start_X + i,Start_Y))     Clear_Point(Start_X + i,Start_Y),      Target_X = Start_X + i, Target_Y = Start_Y,      ++i;
		
		else if(SOUTHEAST == Direction)
				while(Read_Point(Start_X + i,Start_Y - i)) Clear_Point(Start_X + i,Start_Y - i),  Target_X = Start_X + i, Target_Y = Start_Y - i,  ++i;
		
		else if(SOUTH == Direction)
				while(Read_Point(Start_X,Start_Y - i))     Clear_Point(Start_X,Start_Y - i),      Target_X = Start_X,     Target_Y = Start_Y - i,  ++i;
		
		else if(SOUTHWEST == Direction)
				while(Read_Point(Start_X - i,Start_Y - i)) Clear_Point(Start_X - i,Start_Y - i),  Target_X = Start_X - i, Target_Y = Start_Y - i,  ++i;
		
		else if(WEST == Direction)
				while(Read_Point(Start_X - i,Start_Y))     Clear_Point(Start_X - i,Start_Y),      Target_X = Start_X - i, Target_Y = Start_Y,      ++i;
		
		else if(NORTHWEST == Direction)
				while(Read_Point(Start_X - i,Start_Y + i)) Clear_Point(Start_X - i,Start_Y + i),  Target_X = Start_X - i, Target_Y = Start_Y + i,  ++i;
		forward_Position = (i-1) * 10;		//单位为cm
	}
	else if(3 == mode)
	{
		if(Read_Point(Start_X,Start_Y + 1))
		{
			next_Direction = NORTH;
		}
		else if(Read_Point(Start_X + 1,Start_Y + 1))
		{
			next_Direction = NORTHWEST;
		}
		else if(Read_Point(Start_X + 1,Start_Y))
		{
			next_Direction = EAST;
		}
		else if(Read_Point(Start_X + 1,Start_Y - 1))
		{
			next_Direction = SOUTHEAST;
		}
		else if(Read_Point(Start_X,Start_Y - 1))
		{
			next_Direction = SOUTH;
		}
		else if(Read_Point(Start_X - 1,Start_Y - 1))
		{
			next_Direction = SOUTHWEST;
		}
		else if(Read_Point(Start_X - 1,Start_Y))
		{
			next_Direction = WEST;
		}
		else if(Read_Point(Start_X - 1,Start_Y + 1))
		{
			next_Direction = NORTHWEST;
		}
		else		//没路了
		{
			ALL_STOP = 1;
			Target_Angle = 0;
		}
		//printf("%d\n",is_ready);
		//计算角度
		if(!ALL_STOP)
		{
			Target_Angle = (next_Direction - Direction) * 45;
			Direction = next_Direction;
		}
	}
}

void map_move(void)
{
	if(2 == mode)
	{
			move_forward_backword(forward_Position);
			Start_X = Target_X;
			Start_Y = Target_Y;
			//printf("%d %d\n",Start_X,Start_Y);
	}
	else if(3 == mode)
	{
			move_angle(Target_Angle);
	}
}


