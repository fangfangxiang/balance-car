/*
 * camera.h
 *
 *  Created on: 2022年1月27日
 *      Author: 欢悦的骷髅兵
 */

#ifndef CODE_CAMERA_H_
#define CODE_CAMERA_H_
#include "headfile.h"

#define IMAGEH  120  /*!< 摄像头采集高度 */
#define IMAGEW  188   /*!< 摄像头采集宽度 */

#define LCDH    120  /*!< OLED显示高度（用户使用）高度 */
#define LCDW    188  /*!< OLED显示宽度（用户使用）宽度 */

extern uint8 mt9v03x_image[IMAGEH][IMAGEW];


//函数：
short GetOSTU ();//大津法
void get_line();
void Get_Bin_Image (unsigned char mode);
void buxian(int x0,int y0,int x1,int y1);//补线
void buxian2(int x0,int y0,int x1,int y1);//补线并延长至下边
void L_buxian(int x0,int y0,int x1,int y1);//补线并作为左边界
void R_buxian(int x0,int y0,int x1,int y1);//补线并作为右边界
void advanced_regression(int type, int startline1, int endline1, int startline2, int endline2);/*最小二乘法，可以选取两段数据作为拟合****/
void run(int type, int startline, int endline, float parameterB, float parameterA);//***/*最小二乘法  拟合直线函数*/

void lq_sobel (uint8 imageIn[LCDH][LCDW], uint8 imageOut[LCDH][LCDW], unsigned char Threshold);
void lq_sobelAutoThreshold (uint8 imageIn[LCDH][LCDW], uint8 imageOut[LCDH][LCDW]);

void Bin_Image_Filter (void);
void Seek_Road (void);

#endif /* CODE_CAMERA_H_ */
