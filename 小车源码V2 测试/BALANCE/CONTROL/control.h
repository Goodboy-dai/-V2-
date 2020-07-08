#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"


int EXTI15_10_IRQHandler(void);
void Set_Pwm(int motor_a,int motor_b,int motor_c,int motor_d);
void Kinematic_Analysis(float Vx,float Vy,float Vz);


void Xianfu_Pwm(int amplitude);
void Xianfu_Velocity(int amplitude_A,int amplitude_B,int amplitude_C,int amplitude_D);

u32 myabs(long int a);
int Incremental_PI_A (int Encoder,int Target);
int Incremental_PI_B (int Encoder,int Target);
int Incremental_PI_C (int Encoder,int Target);
int Incremental_PI_D (int Encoder,int Target);

void handle_u2(void);


void send_sta(void);
void handle_u3(void);
void send_odoemtry(void);

#endif
