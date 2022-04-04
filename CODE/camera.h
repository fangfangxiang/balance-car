/*
 * camera.h
 *
 *  Created on: 2022��1��27��
 *      Author: ���õ����ñ�
 */

#ifndef CODE_CAMERA_H_
#define CODE_CAMERA_H_
#include "headfile.h"

#define IMAGEH  120  /*!< ����ͷ�ɼ��߶� */
#define IMAGEW  188   /*!< ����ͷ�ɼ���� */

#define LCDH    120  /*!< OLED��ʾ�߶ȣ��û�ʹ�ã��߶� */
#define LCDW    188  /*!< OLED��ʾ��ȣ��û�ʹ�ã���� */

extern uint8 mt9v03x_image[IMAGEH][IMAGEW];


//������
short GetOSTU ();//���
void get_line();
void Get_Bin_Image (unsigned char mode);
void buxian(int x0,int y0,int x1,int y1);//����
void buxian2(int x0,int y0,int x1,int y1);//���߲��ӳ����±�
void L_buxian(int x0,int y0,int x1,int y1);//���߲���Ϊ��߽�
void R_buxian(int x0,int y0,int x1,int y1);//���߲���Ϊ�ұ߽�
void advanced_regression(int type, int startline1, int endline1, int startline2, int endline2);/*��С���˷�������ѡȡ����������Ϊ���****/
void run(int type, int startline, int endline, float parameterB, float parameterA);//***/*��С���˷�  ���ֱ�ߺ���*/

void lq_sobel (uint8 imageIn[LCDH][LCDW], uint8 imageOut[LCDH][LCDW], unsigned char Threshold);
void lq_sobelAutoThreshold (uint8 imageIn[LCDH][LCDW], uint8 imageOut[LCDH][LCDW]);

void Bin_Image_Filter (void);
void Seek_Road (void);

#endif /* CODE_CAMERA_H_ */
