/*
 * dir.c
 *
 *  Created on: 2022��3��9��
 *      Author: ���õ����ñ�
 */

#include"balance.h"
#include"camera.h"
#include"headfile.h"
#include "dir.h"

#define S_MOTOR_PIN   ATOM1_CH1_P33_9       //����������

float err = 0;                //����ƫ��ֵ
float err_next = 0;            //������һ��ƫ��ֵ
float err_last = 0;            //��������ǰ��ƫ��ֵ
float increment = 0;
float kiadd = 0;
int duty = 750;

float Kp = 2;
float Ki = 0;
float Kd = 0;            //������������֡�΢��ϵ��

extern float TempAngle;

void dir_pd(void)
{
    err= get_TempAngle(3,70);

    kiadd += err;
    if(kiadd<-380)      kiadd=-380; //�޷�
    else if(kiadd>380)  kiadd= 380; //�޷�

    increment = Kp*err + Ki*kiadd + Kd*(err - 2*err_next + err_last);

    err_last=err_next;
    err_next=err;

    duty = 750 - increment;
    if(duty>900) duty=900;
    if(duty<600) duty=600;
    lcd_showuint16(0,5,duty);

    pwm_duty(S_MOTOR_PIN,duty);

}
