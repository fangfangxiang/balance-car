/*
 * balance.h
 *
 *  Created on: 2022年1月28日
 *      Author: 欢悦的骷髅兵
 */

#ifndef CODE_BALANCE_H_
#define CODE_BALANCE_H_
#include "headfile.h"


//函数：
void Balance(void);//主要函数

float X_balance_Control(float Angle,float Angle_Zero,float Gyro);//角度环
float Velocity_Control(int encoder);//动量轮速度环
int SBB_Get_BalancePID(float Angle,float Gyro);//舵机方向环
int SBB_Get_MotorPI (uint16 Encoder,uint16 Target);//速度环



#endif /* CODE_BALANCE_H_ */
