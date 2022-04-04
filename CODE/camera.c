/*
 * camera.c
 *
 *  Created on: 2022��1��27��
 *      Author: ���õ����ñ�
 */
#include "headfile.h"
#include "camera.h"
#define IMAGEH  120  /*!< ����ͷ�ɼ��߶� */
#define IMAGEW  188   /*!< ����ͷ�ɼ���� */
#define LCDH    120  /*!< OLED��ʾ�߶ȣ��û�ʹ�ã��߶� */
#define LCDW    188  /*!< OLED��ʾ��ȣ��û�ʹ�ã���� */

uint8 Bin_Image[LCDH][LCDW];/** ��ֵ��������OLED��ʾ������ */
uint8 Use_Image[LCDH][LCDW];/** ����Ϊ������� */
uint8 LCenter[LCDH];
uint8 R_black[LCDH];
uint8 L_black[LCDH];


sint16 OFFSET0 = 0;      //��Զ������������ֵ�ۺ�ƫ����
sint16 OFFSET1 = 0;      //�ڶ���
sint16 OFFSET2 = 0;      //�����������
sint16 TXV = 0;          //���ε���߶ȣ��Ҹ߶�

float parameterA ; //��С���˷� �ؾ�
float parameterB;  //б��

/*      ͼ���ֵ��
 *    0��ʹ�ô����ֵ
 *    1��ʹ��ƽ����ֵ
 *    2: sobel ���ӸĽ���  �ֶ���ֵ��ͬʱ�����Ϊ��ȡ���ص�ͼ��
 *    3��sobel ���ӸĽ���   ��̬��ֵ��ͬʱ�����Ϊ��ȡ���ص�ͼ��
 */

float temp1=0,temp2=0,temp3=0;
int get_TempAngle(int start,int end) //  ��ȡƫ��
{
    int tempangle = 0;
    for(int i=start;i<=end;i++)
    {
        tempangle = tempangle+(LCenter[i] - 94);

    }
    tempangle = tempangle/(end - start);


//    if(temp1-temp2 < 20 && temp1 -temp2 >-20 && temp1-temp3 < 20 && temp1 -temp3 >-20 && temp3-temp2 < 20 && temp3 -temp2 >-20)
//    {
//        tempangle = temp1;
//        temp3 = temp2;
//        temp2 = temp1;
//    }
//    else
//    {
//        tempangle = temp2;
//    }

    return tempangle;
}


 void Get_Bin_Image (unsigned char mode)
{
    unsigned short i = 0, j = 0;
    unsigned short Threshold = 0;
    unsigned long tv = 0;

    if (mode == 0)//��ֵ���******************************
    {
          Threshold = GetOSTU();  //�����ֵ
          //lcd_showuint16(0,6,Threshold);
    }

    if (mode == 1)//��ֵ���� ƽ��ֵ��20 (20������Ҫ�޸�)********************************
    {
        //�ۼ�
        for (i = 0; i < LCDH; i++)
        {
            for (j = 0; j < LCDW; j++)
            {
                tv += mt9v03x_image[i][j];   //�ۼ�
            }
        }
        Threshold =(unsigned short)(tv / LCDH / LCDW);   //��ƽ��ֵ,����Խ��ԽС��ȫ��Լ35��������ĻԼ160��һ������´�Լ100
        Threshold = Threshold + 20;      //�˴���ֵ���ã����ݻ����Ĺ������趨
    }

    else if (mode == 2)//�Լ�����ֵ***********************
    {
        Threshold = 80;                          //�ֶ�������ֵ
        lq_sobel(mt9v03x_image, Bin_Image, (unsigned char) Threshold);

        return;
    }

    else if (mode == 3) /* ʹ�����ص����������ص�֮�͵�һ������    ��Ϊ��ֵ  */
    {
        lq_sobelAutoThreshold(mt9v03x_image, Bin_Image);  //��̬������ֵ
        return;
    }


        /* ��ֵ�� */
       for (i = 0; i < LCDH; i++)
        {
            for (j = 0; j < LCDW; j++)
            {
                if (mt9v03x_image[i][j] > Threshold) //��ֵԽ����ʾ������Խ�࣬��ǳ��ͼ��Ҳ����ʾ����
                    {
                    Bin_Image[i][j] = 1;//��ɫ
                    Use_Image[119-i][187-j] = 1;
                    }
                else
                    {
                    Bin_Image[i][j] = 0;//��ɫ
                    Use_Image[119-i][187-j] = 0;
                    }
            }
        }



}


/*//  ��ȡ���ߣ�&���ұ߽�*/
void get_line()
{
   int startpoint = IMAGEW/2;
   int i,j;
   for(int i=IMAGEH;i>=1;i--)
  {

         int fl=0, fr = 0;

         L_black[120-i] = 188;
         for(j=startpoint;j>1;j--)
         {
                   if(Bin_Image[i][j-2]==0 && Bin_Image[i][j-1]==0 && Bin_Image[i][j]==1)
                   {
                     L_black[120-i] = 188-j;       // �ҵ���ߺڵ�
                     fl = 1;
                     break;
                   }
          }



         R_black[120-i] = 1;
         for(j=startpoint;j<IMAGEW;j++)          // ���м����ұ�������Ѱ�Һڵ�
         {
               if(Bin_Image[i][j]==1 && Bin_Image[i][j+1]==0 && Bin_Image[i][j+2]==0)
               {
                   R_black[120-i] = 188 -j;         //�ҵ��ұߺڵ�
                   fr = 1;
                 break;
               }

         }

         if(fl == 0 && fr == 0 && i < 117)
             LCenter[120-i] = LCenter[120-i-1];
         else
             LCenter[120-i]= (L_black[120-i] + R_black[120-i])/2 ;
         startpoint = 188 - LCenter[120-i];

  }
}


void buxian(int x0,int y0,int x1,int y1)//����
{
     int x,i;
     float K = (x0-x1) * 1.0 / (y0-y1);
     if(K>20) K=20;
     if(K<-20) K=-20;
     if(y1 > y0)
     {
         for(i=y0+1;i< y1;i++)
         {
             x = x0+(i-y0)*K;
             Bin_Image[120-i][ 188-x ] = 0;
            Bin_Image[120-i][ 188-x+1 ] = 0;
    //        Use_Image[i-1][x-1] = 0;
    //        Use_Image[i-1][x-2] = 0;
         }
     }
     else
     {
         for(i=y1+1;i< y0;i++)
         {
             x = x0+(i-y0)*K;
             Bin_Image[120-i][ 188-x ] = 0;
            Bin_Image[120-i][ 188-x+1 ] = 0;
    //        Use_Image[i-1][x-1] = 0;
    //        Use_Image[i-1][x-2] = 0;
         }
     }

}

void buxian2(int x0,int y0,int x1,int y1)//���߲��ӳ�
{
     int x,i;
     float K = (x0-x1) * 1.0 / (y0-y1);
     if(K>20) K=20;
     if(K<-20) K=-20;
     for(i=2;i< 110;i++)
     {
         x = x0+(i-y0)*K;
         Bin_Image[120-i][ 188-x ] = 0;
        Bin_Image[120-i][ 188-x+1 ] = 0;
     }

}





void R_buxian(int x0,int y0,int x1,int y1)//���߲���Ϊ�ұ߽�
{
     int x,i;
     float K = (x0-x1) * 1.0 / (y0-y1);
     if(K>20) K=20;
     if(K<-20) K=-20;
     for(i=y0+1;i< y1;i++)
     {
         x = x0+(i-y0)*K;
         Bin_Image[120-i][ 188-x ] = 0;
        Bin_Image[120-i][ 188-x+1 ] = 0;
        R_black[i] = x;
     }

}

void L_buxian(int x0,int y0,int x1,int y1)//���߲���Ϊ��߽�
{
     int x,i;
     float K = (x0-x1) * 1.0 / (y0-y1);
     if(K>20) K=20;
     if(K<-20) K=-20;
     for(i=y0+1;i< y1;i++)
     {
         x = x0+(i-y0)*K;
         Bin_Image[120-i][ 188-x ] = 0;
        Bin_Image[120-i][ 188-x+1 ] = 0;
        L_black[i] = x;
     }
}



/*���***/
short GetOSTU ()
{
    signed short i, j;
    unsigned long Amount = 0;
    unsigned long PixelBack = 0;
    unsigned long PixelshortegralBack = 0;
    unsigned long Pixelshortegral = 0;
    signed long PixelshortegralFore = 0;
    signed long PixelFore = 0;
    float OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // ��䷽��;
    signed short MinValue, MaxValue;
    signed short Threshold = 0;
    unsigned char HistoGram[256];              //

    for (j = 0; j < 256; j++)
        HistoGram[j] = 0; //��ʼ���Ҷ�ֱ��ͼ

    for (j = 0; j < LCDH; j++)
    {
        for (i = 0; i < LCDW; i++)
        {
            HistoGram[mt9v03x_image[j][i]]++; //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
        }
    }

    for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++);        //��ȡ��С�Ҷȵ�ֵ
    for (MaxValue = 255; MaxValue > MinValue && HistoGram[MinValue] == 0; MaxValue--); //��ȡ���Ҷȵ�ֵ

    if (MaxValue == MinValue)
        return MaxValue;         // ͼ����ֻ��һ����ɫ
    if (MinValue + 1 == MaxValue)
        return MinValue;        // ͼ����ֻ�ж�����ɫ

    for (j = MinValue; j <= MaxValue; j++)
        Amount += HistoGram[j];        //  ��������

    Pixelshortegral = 0;
    for (j = MinValue; j <= MaxValue; j++)
    {
        Pixelshortegral += HistoGram[j] * j;        //�Ҷ�ֵ����
    }
    SigmaB = -1;
    for (j = MinValue; j < MaxValue; j++)
    {
        PixelBack = PixelBack + HistoGram[j];     //ǰ�����ص���
        PixelFore = Amount - PixelBack;           //�������ص���
        OmegaBack = (float) PixelBack / Amount;   //ǰ�����ذٷֱ�
        OmegaFore = (float) PixelFore / Amount;   //�������ذٷֱ�
        PixelshortegralBack += HistoGram[j] * j;  //ǰ���Ҷ�ֵ
        PixelshortegralFore = Pixelshortegral - PixelshortegralBack;  //�����Ҷ�ֵ
        MicroBack = (float) PixelshortegralBack / PixelBack;   //ǰ���ҶȰٷֱ�
        MicroFore = (float) PixelshortegralFore / PixelFore;   //�����ҶȰٷֱ�
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);   //������䷽��
        if (Sigma > SigmaB)                    //����������䷽��g //�ҳ������䷽���Լ���Ӧ����ֵ
        {
            SigmaB = Sigma;
            Threshold = j;
        }
    }
    return Threshold;                        //���������ֵ;
}



/* @brief    ����soble���ؼ�����ӵ�һ�ֱ��ؼ��
 *
 * @param    imageIn    ��������
 *           imageOut   �������      ����Ķ�ֵ����ı�����Ϣ
 */
void lq_sobel (uint8 imageIn[LCDH][LCDW], uint8 imageOut[LCDH][LCDW], unsigned char Threshold)
{
    /** ����˴�С */
    short KERNEL_SIZE = 3;
    short xStart = KERNEL_SIZE / 2;
    short xEnd = LCDW - KERNEL_SIZE / 2;
    short yStart = KERNEL_SIZE / 2;
    short yEnd = LCDH - KERNEL_SIZE / 2;
    short i, j, k;
    short temp[4];

    for (i = yStart; i < yEnd; i++)
    {
        for (j = xStart; j < xEnd; j++)
        {
            /* ���㲻ͬ�����ݶȷ�ֵ  */
            temp[0] = -(short) imageIn[i - 1][j - 1] + (short) imageIn[i - 1][j + 1]     //{{-1, 0, 1},
            - (short) imageIn[i][j - 1] + (short) imageIn[i][j + 1]        // {-1, 0, 1},
            - (short) imageIn[i + 1][j - 1] + (short) imageIn[i + 1][j + 1];    // {-1, 0, 1}};

            temp[1] = -(short) imageIn[i - 1][j - 1] + (short) imageIn[i + 1][j - 1]     //{{-1, -1, -1},
            - (short) imageIn[i - 1][j] + (short) imageIn[i + 1][j]       // { 0,  0,  0},
            - (short) imageIn[i - 1][j + 1] + (short) imageIn[i + 1][j + 1];    // { 1,  1,  1}};

            temp[2] = -(short) imageIn[i - 1][j] + (short) imageIn[i][j - 1]       //  0, -1, -1
            - (short) imageIn[i][j + 1] + (short) imageIn[i + 1][j]       //  1,  0, -1
            - (short) imageIn[i - 1][j + 1] + (short) imageIn[i + 1][j - 1];    //  1,  1,  0

            temp[3] = -(short) imageIn[i - 1][j] + (short) imageIn[i][j + 1]       // -1, -1,  0
            - (short) imageIn[i][j - 1] + (short) imageIn[i + 1][j]       // -1,  0,  1
            - (short) imageIn[i - 1][j - 1] + (short) imageIn[i + 1][j + 1];    //  0,  1,  1

            temp[0] = abs(temp[0]);
            temp[1] = abs(temp[1]);
            temp[2] = abs(temp[2]);
            temp[3] = abs(temp[3]);

            /* �ҳ��ݶȷ�ֵ���ֵ  */
            for (k = 1; k < 4; k++)
            {
                if (temp[0] < temp[k])
                {
                    temp[0] = temp[k];
                }
            }

            if (temp[0] > Threshold)
            {
                imageOut[i][j] = 1;
            }
            else
            {
                imageOut[i][j] = 0;
            }
        }
    }
}



/* @brief    ����soble���ؼ�����ӵ�һ���Զ���ֵ���ؼ��
 *
 * @param    imageIn    ��������
 *           imageOut   �������      ����Ķ�ֵ����ı�����Ϣ
 */
void lq_sobelAutoThreshold (uint8 imageIn[LCDH][LCDW], uint8 imageOut[LCDH][LCDW])
{
    /** ����˴�С */
    short KERNEL_SIZE = 3;
    short xStart = KERNEL_SIZE / 2;
    short xEnd = LCDW - KERNEL_SIZE / 2;
    short yStart = KERNEL_SIZE / 2;
    short yEnd = LCDH - KERNEL_SIZE / 2;
    short i, j, k;
    short temp[4];
    for (i = yStart; i < yEnd; i++)
    {
        for (j = xStart; j < xEnd; j++)
        {
            /* ���㲻ͬ�����ݶȷ�ֵ  */
            temp[0] = -(short) imageIn[i - 1][j - 1] + (short) imageIn[i - 1][j + 1]     //{{-1, 0, 1},
            - (short) imageIn[i][j - 1] + (short) imageIn[i][j + 1]       // {-1, 0, 1},
            - (short) imageIn[i + 1][j - 1] + (short) imageIn[i + 1][j + 1];    // {-1, 0, 1}};

            temp[1] = -(short) imageIn[i - 1][j - 1] + (short) imageIn[i + 1][j - 1]     //{{-1, -1, -1},
            - (short) imageIn[i - 1][j] + (short) imageIn[i + 1][j]       // { 0,  0,  0},
            - (short) imageIn[i - 1][j + 1] + (short) imageIn[i + 1][j + 1];    // { 1,  1,  1}};

            temp[2] = -(short) imageIn[i - 1][j] + (short) imageIn[i][j - 1]       //  0, -1, -1
            - (short) imageIn[i][j + 1] + (short) imageIn[i + 1][j]       //  1,  0, -1
            - (short) imageIn[i - 1][j + 1] + (short) imageIn[i + 1][j - 1];    //  1,  1,  0

            temp[3] = -(short) imageIn[i - 1][j] + (short) imageIn[i][j + 1]       // -1, -1,  0
            - (short) imageIn[i][j - 1] + (short) imageIn[i + 1][j]       // -1,  0,  1
            - (short) imageIn[i - 1][j - 1] + (short) imageIn[i + 1][j + 1];    //  0,  1,  1

            temp[0] = abs(temp[0]);
            temp[1] = abs(temp[1]);
            temp[2] = abs(temp[2]);
            temp[3] = abs(temp[3]);

            /* �ҳ��ݶȷ�ֵ���ֵ  */
            for (k = 1; k < 4; k++)
            {
                if (temp[0] < temp[k])
                {
                    temp[0] = temp[k];
                }
            }

            /* ʹ�����ص����������ص�֮�͵�һ������    ��Ϊ��ֵ  */
            temp[3] = (short) imageIn[i - 1][j - 1] + (short) imageIn[i - 1][j] + (short) imageIn[i - 1][j + 1]
                    + (short) imageIn[i][j - 1] + (short) imageIn[i][j] + (short) imageIn[i][j + 1]
                    + (short) imageIn[i + 1][j - 1] + (short) imageIn[i + 1][j] + (short) imageIn[i + 1][j + 1];

            if (temp[0] > temp[3] / 12.0f)
            {
                imageOut[i][j] = 1;
            }
            else
            {
                imageOut[i][j] = 0;
            }

        }
    }
}



/*�������*/
void Bin_Image_Filter (void)
{
    sint16 nr; //��
    sint16 nc; //��

    for (nr = 1; nr < LCDH - 1; nr++)
    {
        for (nc = 1; nc < LCDW - 1; nc = nc + 1)
        {
            if ((Bin_Image[nr][nc] == 0)
                    && (Bin_Image[nr - 1][nc] + Bin_Image[nr + 1][nc] + Bin_Image[nr][nc + 1] + Bin_Image[nr][nc - 1] > 2))
            {
                Bin_Image[nr][nc] = 1;
            }
            else if ((Bin_Image[nr][nc] == 1)
                    && (Bin_Image[nr - 1][nc] + Bin_Image[nr + 1][nc] + Bin_Image[nr][nc + 1] + Bin_Image[nr][nc - 1] < 2))
            {
                Bin_Image[nr][nc] = 0;
            }
        }
    }
}



/* Ѱ�Ұ�ɫ����ƫ��ֵ     8--56��                                       *
 *  ��    ע�����м�Ϊ0������һ���Ҳ��һ����ֵ����1�����                *
 *            ��������ӵ�һ�п�ʼ�������ڶ��н�����                        *
 *            ������Ϊ��������ֵԽ��˵��Խƫ��ߣ�                        *
 *            ������Ϊ��������ֵԽ��˵��Խƫ�ұߡ�                        *
 ***************************************************************************/
#define MAX_COL LCDW
void Seek_Road (void)
{
    sint16 nr; //��
    sint16 nc; //��
    sint16 temp = 0; //��ʱ��ֵ
    temp = 0;
    for (nr = 8; nr < 24; nr++)
    {
        for (nc = MAX_COL / 2; nc < MAX_COL; nc = nc + 1)
        {
            if (Bin_Image[nr][nc])
            {
                ++temp;
            }
        }
        for (nc = 0; nc < MAX_COL / 2; nc = nc + 1)
        {
            if (Bin_Image[nr][nc])
            {
                --temp;
            }
        }
    }
    OFFSET0 = temp;
    temp = 0;
    for (nr = 24; nr < 40; nr++)
    {
        for (nc = MAX_COL / 2; nc < MAX_COL; nc = nc + 1)
        {
            if (Bin_Image[nr][nc])
            {
                ++temp;
            }
        }
        for (nc = 0; nc < MAX_COL / 2; nc = nc + 1)
        {
            if (Bin_Image[nr][nc])
            {
                --temp;
            }
        }
    }
    OFFSET1 = temp;
    temp = 0;
    for (nr = 40; nr < 56; nr++)
    {
        for (nc = MAX_COL / 2; nc < MAX_COL; nc = nc + 1)
        {
            if (Bin_Image[nr][nc])
            {
                ++temp;
            }
        }
        for (nc = 0; nc < MAX_COL / 2; nc = nc + 1)
        {
            if (Bin_Image[nr][nc])
            {
                --temp;
            }
        }
    }
    OFFSET2 = temp;
    return;
}
