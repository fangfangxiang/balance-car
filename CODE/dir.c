/*
 * dir.c
 *
 *  Created on: 2022年3月9日
 *      Author: 欢悦的骷髅兵
 */

#include"balance.h"
#include"camera.h"
#include"headfile.h"
#include "dir.h"

#define S_MOTOR_PIN   ATOM1_CH1_P33_9       //定义舵机引脚

float err = 0;                //定义偏差值
float err_next = 0;            //定义上一个偏差值
float err_last = 0;            //定义最上前的偏差值
float increment = 0;
float kiadd = 0;
int duty = 750;

float Kp = 2;
float Ki = 0;
float Kd = 0;            //定义比例、积分、微分系数

extern float TempAngle;

void dir_pd(void)
{
    err= get_TempAngle(3,70);

    kiadd += err;
    if(kiadd<-380)      kiadd=-380; //限幅
    else if(kiadd>380)  kiadd= 380; //限幅

    increment = Kp*err + Ki*kiadd + Kd*(err - 2*err_next + err_last);

    err_last=err_next;
    err_next=err;

    duty = 750 - increment;
    if(duty>900) duty=900;
    if(duty<600) duty=600;
    lcd_showuint16(0,5,duty);

    pwm_duty(S_MOTOR_PIN,duty);

}
