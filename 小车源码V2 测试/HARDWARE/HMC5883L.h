#ifndef HMC5883
#define HMC5883


#define HMC5883_Addr 0x1E


//IO方向设置
#define sda_in()  {GPIOA->CRL&=0XFFFF0FFF;GPIOA->CRL|=0x00008000;}
#define sda_out() {GPIOA->CRL&=0XFFFF0FFF;GPIOA->CRL|=0x00003000;}

//IO操作函数	 
#define iic_scl   PAout(2) //SCL
#define iic_sda    PAout(3) //SDA	 
#define read_sda   PAin(3)  //输入SDA 

void iic_Start(void);
void iic_Stop(void);
void iic_Ack(void);
void iic_NAck(void);
unsigned char iic_Wait_Ack(void);
void iic_Send_Byte(unsigned char txd);
unsigned char iic_Read_Byte(unsigned char ack);

void HMC5883_Init(void);
unsigned char Single_Write_5883(unsigned char reg, unsigned char dat);
unsigned char Single_Read_5883(unsigned reg);
unsigned char Read_5883(unsigned char addr, unsigned char reg, unsigned char len, unsigned char *buf);


#endif 

