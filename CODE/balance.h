/*
 * balance.h
 *
 *  Created on: 2022��1��28��
 *      Author: ���õ����ñ�
 */

#ifndef CODE_BALANCE_H_
#define CODE_BALANCE_H_
#include "headfile.h"


//������
void Balance(void);//��Ҫ����

float X_balance_Control(float Angle,float Angle_Zero,float Gyro);//�ǶȻ�
float Velocity_Control(int encoder);//�������ٶȻ�
int SBB_Get_BalancePID(float Angle,float Gyro);//�������
int SBB_Get_MotorPI (uint16 Encoder,uint16 Target);//�ٶȻ�



#endif /* CODE_BALANCE_H_ */
