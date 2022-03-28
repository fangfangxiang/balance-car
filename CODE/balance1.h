/*
 * Balance.h
 *
 *  Created on: 2022Äê1ÔÂ29ÈÕ
 *      Author: guoguo
 */

#ifndef CODE_BALANCE_H_
#define CODE_BALANCE_H_
#include "headfile.h"

void encoder_Flywheel_get();
float flywheel_balance(float angle , float gyro);
float flywheel_speed(int16 encoder);
void flywheel_control_Brush();
void flywheel_control_Brushless();

extern float flywheel_balance_P;
extern float flywheel_balance_I;
extern float flywheel_balance_D;
extern float flywheel_speed_P;
extern float flywheel_speed_I;
extern int16 flywheel_duty;
extern int16 encoder_flywheel;
extern float error_my;
extern float Angle;
extern float Angle_Zero;
extern int16 limit_speed;



#endif /* CODE_BALANCE_H_ */
