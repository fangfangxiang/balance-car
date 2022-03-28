/*
 * tuoluoyi.h
 *
 *  Created on: 2021年11月4日
 *      Author: 廖杰民
 */

#ifndef CODE_TUOLUOYI_H_
#define CODE_TUOLUOYI_H_
#include "headfile.h"

extern void IMU_getValues(void);
extern void IMU_quaterToEulerianAngles(void);
extern  void IMU_AHRSupdate_noMagnetic(float , float , float , float , float , float );
extern float invSqrt(float );

extern float yaw,pitch,roll;






#endif /* CODE_TUOLUOYI_H_ */
