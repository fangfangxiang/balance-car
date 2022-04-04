/*
 * camera.c
 *
 *  Created on: 2022年1月27日
 *      Author: 欢悦的骷髅兵
 */
#include "headfile.h"
#include "camera.h"
#define IMAGEH  120  /*!< 摄像头采集高度 */
#define IMAGEW  188   /*!< 摄像头采集宽度 */
#define LCDH    120  /*!< OLED显示高度（用户使用）高度 */
#define LCDW    188  /*!< OLED显示宽度（用户使用）宽度 */

uint8 Bin_Image[LCDH][LCDW];/** 二值化后用于OLED显示的数据 */
uint8 Use_Image[LCDH][LCDW];/** 右下为坐标零点 */
uint8 LCenter[LCDH];
uint8 R_black[LCDH];
uint8 L_black[LCDH];


sint16 OFFSET0 = 0;      //最远处，赛道中心值综合偏移量
sint16 OFFSET1 = 0;      //第二格
sint16 OFFSET2 = 0;      //最近，第三格
sint16 TXV = 0;          //梯形的左高度，右高度

float parameterA ; //最小二乘法 截距
float parameterB;  //斜率

/*      图像二值化
 *    0：使用大津法阈值
 *    1：使用平均阈值
 *    2: sobel 算子改进型  手动阈值，同时输出改为提取边沿的图像
 *    3：sobel 算子改进型   动态阈值，同时输出改为提取边沿的图像
 */

float temp1=0,temp2=0,temp3=0;
int get_TempAngle(int start,int end) //  获取偏差
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

    if (mode == 0)//阈值大津法******************************
    {
          Threshold = GetOSTU();  //大津法阈值
          //lcd_showuint16(0,6,Threshold);
    }

    if (mode == 1)//阈值等于 平均值＋20 (20根据需要修改)********************************
    {
        //累加
        for (i = 0; i < LCDH; i++)
        {
            for (j = 0; j < LCDW; j++)
            {
                tv += mt9v03x_image[i][j];   //累加
            }
        }
        Threshold =(unsigned short)(tv / LCDH / LCDW);   //求平均值,光线越暗越小，全黑约35，对着屏幕约160，一般情况下大约100
        Threshold = Threshold + 20;      //此处阈值设置，根据环境的光线来设定
    }

    else if (mode == 2)//自己定阈值***********************
    {
        Threshold = 80;                          //手动调节阈值
        lq_sobel(mt9v03x_image, Bin_Image, (unsigned char) Threshold);

        return;
    }

    else if (mode == 3) /* 使用像素点邻域内像素点之和的一定比例    作为阈值  */
    {
        lq_sobelAutoThreshold(mt9v03x_image, Bin_Image);  //动态调节阈值
        return;
    }


        /* 二值化 */
       for (i = 0; i < LCDH; i++)
        {
            for (j = 0; j < LCDW; j++)
            {
                if (mt9v03x_image[i][j] > Threshold) //数值越大，显示的内容越多，较浅的图像也能显示出来
                    {
                    Bin_Image[i][j] = 1;//白色
                    Use_Image[119-i][187-j] = 1;
                    }
                else
                    {
                    Bin_Image[i][j] = 0;//黑色
                    Use_Image[119-i][187-j] = 0;
                    }
            }
        }



}


/*//  提取中线，&左右边界*/
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
                     L_black[120-i] = 188-j;       // 找到左边黑点
                     fl = 1;
                     break;
                   }
          }



         R_black[120-i] = 1;
         for(j=startpoint;j<IMAGEW;j++)          // 从中间向右边搜索，寻找黑点
         {
               if(Bin_Image[i][j]==1 && Bin_Image[i][j+1]==0 && Bin_Image[i][j+2]==0)
               {
                   R_black[120-i] = 188 -j;         //找到右边黑点
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


void buxian(int x0,int y0,int x1,int y1)//补线
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

void buxian2(int x0,int y0,int x1,int y1)//补线并延长
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





void R_buxian(int x0,int y0,int x1,int y1)//补线并作为右边界
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

void L_buxian(int x0,int y0,int x1,int y1)//补线并作为左边界
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



/*大津法***/
short GetOSTU ()
{
    signed short i, j;
    unsigned long Amount = 0;
    unsigned long PixelBack = 0;
    unsigned long PixelshortegralBack = 0;
    unsigned long Pixelshortegral = 0;
    signed long PixelshortegralFore = 0;
    signed long PixelFore = 0;
    float OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差;
    signed short MinValue, MaxValue;
    signed short Threshold = 0;
    unsigned char HistoGram[256];              //

    for (j = 0; j < 256; j++)
        HistoGram[j] = 0; //初始化灰度直方图

    for (j = 0; j < LCDH; j++)
    {
        for (i = 0; i < LCDW; i++)
        {
            HistoGram[mt9v03x_image[j][i]]++; //统计灰度级中每个像素在整幅图像中的个数
        }
    }

    for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++);        //获取最小灰度的值
    for (MaxValue = 255; MaxValue > MinValue && HistoGram[MinValue] == 0; MaxValue--); //获取最大灰度的值

    if (MaxValue == MinValue)
        return MaxValue;         // 图像中只有一个颜色
    if (MinValue + 1 == MaxValue)
        return MinValue;        // 图像中只有二个颜色

    for (j = MinValue; j <= MaxValue; j++)
        Amount += HistoGram[j];        //  像素总数

    Pixelshortegral = 0;
    for (j = MinValue; j <= MaxValue; j++)
    {
        Pixelshortegral += HistoGram[j] * j;        //灰度值总数
    }
    SigmaB = -1;
    for (j = MinValue; j < MaxValue; j++)
    {
        PixelBack = PixelBack + HistoGram[j];     //前景像素点数
        PixelFore = Amount - PixelBack;           //背景像素点数
        OmegaBack = (float) PixelBack / Amount;   //前景像素百分比
        OmegaFore = (float) PixelFore / Amount;   //背景像素百分比
        PixelshortegralBack += HistoGram[j] * j;  //前景灰度值
        PixelshortegralFore = Pixelshortegral - PixelshortegralBack;  //背景灰度值
        MicroBack = (float) PixelshortegralBack / PixelBack;   //前景灰度百分比
        MicroFore = (float) PixelshortegralFore / PixelFore;   //背景灰度百分比
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);   //计算类间方差
        if (Sigma > SigmaB)                    //遍历最大的类间方差g //找出最大类间方差以及对应的阈值
        {
            SigmaB = Sigma;
            Threshold = j;
        }
    }
    return Threshold;                        //返回最佳阈值;
}



/* @brief    基于soble边沿检测算子的一种边沿检测
 *
 * @param    imageIn    输入数组
 *           imageOut   输出数组      保存的二值化后的边沿信息
 */
void lq_sobel (uint8 imageIn[LCDH][LCDW], uint8 imageOut[LCDH][LCDW], unsigned char Threshold)
{
    /** 卷积核大小 */
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
            /* 计算不同方向梯度幅值  */
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

            /* 找出梯度幅值最大值  */
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



/* @brief    基于soble边沿检测算子的一种自动阈值边沿检测
 *
 * @param    imageIn    输入数组
 *           imageOut   输出数组      保存的二值化后的边沿信息
 */
void lq_sobelAutoThreshold (uint8 imageIn[LCDH][LCDW], uint8 imageOut[LCDH][LCDW])
{
    /** 卷积核大小 */
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
            /* 计算不同方向梯度幅值  */
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

            /* 找出梯度幅值最大值  */
            for (k = 1; k < 4; k++)
            {
                if (temp[0] < temp[k])
                {
                    temp[0] = temp[k];
                }
            }

            /* 使用像素点邻域内像素点之和的一定比例    作为阈值  */
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



/*过滤噪点*/
void Bin_Image_Filter (void)
{
    sint16 nr; //行
    sint16 nc; //列

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



/* 寻找白色区域偏差值     8--56行                                       *
 *  备    注：以中间为0，左侧减一，右侧加一，数值代表1的面积                *
 *            计算区域从第一行开始到倒数第二行结束。                        *
 *            如果面积为负数，数值越大说明越偏左边；                        *
 *            如果面积为正数，数值越大说明越偏右边。                        *
 ***************************************************************************/
#define MAX_COL LCDW
void Seek_Road (void)
{
    sint16 nr; //行
    sint16 nc; //列
    sint16 temp = 0; //临时数值
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
