/*
 * imageprocess.c
 *
 *  Created on: 2022年1月27日
 *      Author: 欢悦的骷髅兵
 */

#include"headfile.h"
#include "imageprocess.h"
#include "camera.h"

#define LCDH  120  /*!< 摄像头采集高度 */
#define LCDW  188   /*!< 摄像头采集宽度 */
extern float TempAngle;

extern uint8 Bin_Image[120][188];
extern uint8 Use_Image[120][188];
extern uint8 L_black[LCDH];
extern uint8 R_black[LCDH];
extern uint8 LCenter [LCDH];

int leftx[3],lefty[3],rightx[3],righty[3];//上中下拐点
int middle[4];//突变点
int middlex[4],middley[4];


int chuku_times = 0;
int flag_cont = 0;//用完记得清零，好重复使用


/********************************标志**********************************************/
int flag_judge_chuku = 0;
int flag_havechuku = 0;

int flag_shizi = 0;
int flag_sancha = 0;

int flag_huandao = 0;//调试那个阶段就改成几，入环和出环调成1
int flag_ruhuan = 0;
int flag_chuhuan = 0;
int flag_rusancha = 0;

int flag_starting_line = 0;
int flag_ruku = 0;
int flag_find_cheku ;
/************************************************************************************/



/*出库拉线右拐*//**********************************************************************************************************/
void chuku(void)
{
    int y , x;
    int y1 = 2;
    int x1 =3;
    righty[0] = 2;

    if (R_black[2] != 1 && R_black[3] != 1 && R_black[4] != 1)//先找拐点
    {
        for (int i = 1; i < 110; i++)
        {
            if (abs(R_black[i] - R_black[i - 1]) < 5 && abs(R_black[i + 1] - R_black[i]) < 5 && R_black[i + 2] - R_black[i + 1] < 0
                    && i + 1 >= 2 && R_black[i + 1] > 2 && i + 1 < 90)
            {
                righty[0] = i + 1;
                rightx[0] = R_black[i+1];
                break;
             }
         }
     }

/*****************************************************拉线阶段**********************************************************/
if(righty[0] < 30)//要调   //判断右拐点位置，并获得有拐点上方黑色y1
{

    if (Use_Image[10][3] != 0)// 白  拐点已近很下面了
    {
        for (y = 11; y < 110; y++)
        {
            if (Use_Image[y][ 3] == 0)
            {
                y1 = y;
                break;
            }
        }
    }
    else //黑
    {
        int cnt = 0;
        for (y = 4; y < 110; y++)
        {
            if (Use_Image[y][ 3] != 0 && Use_Image[y - 1][ 3] == 0) //上白下黑
            {
                cnt = 1;
            }

            if (Use_Image[y][ 3] == 0 && Use_Image[y - 1][ 3] != 0 && cnt == 1) //上黑下白--y1
            {
                cnt = 2;
                y1 = y;
                break;
            }
        }

//        for (y = y1; y < 110; y++)
//        {
//            if (Use_Image[y1][ 3] == 0)
//            break;
//        }
    }


    for (x = 3; x < 185; x++)//获取右上拉线点
    {
        if (Use_Image[y1][ x] != 0)//白  上边黑线右斜
        {
            righty[1] = y1;
            rightx[1] = x;
            break;
        }

        if (x > 93)//上黑边界左斜
        {
            righty[1] = y1;
            rightx[1] = 0;
            break;
        }
    }


    for (y = 5; y < 70; y++)//  获得左下边拉线点
    {
        if (Use_Image[y][ 185] != 0) //白
        {
            lefty[0] = y;
            leftx[0] = 185;
            break;
        }
        if (y > 10)
        {
            lefty[0] = 10;
            leftx[0] = 185;
            break;
        }
     }

    for(int i=righty[0];i<righty[1];i++)//清除斑马线
    {
        for(int j=rightx[0] ; j<185; j++)
        {
            Bin_Image[119-i][187-j] = 1;
        }
    }

    flag_judge_chuku = 1;
    buxian(leftx[0],lefty[0],rightx[1],righty[1]);
    get_line();
 }
/*****************************************************拉线阶段***************************************************/

/************************************************判断已出库*****************************************************************/
if(flag_judge_chuku == 1)
{
    int out = 0;  //出库成功判断，不好用，应该是刚启动时全屏为黑//要调或者改方案
    for( int i=110;i>=30;i--)
    {
        if(Bin_Image[i][10] ==0  && Bin_Image[i][180] ==0) //两边为黑
        {
            out ++;
        }
    }

    if(out > 60)
   {
    chuku_times++;
    }

    if(chuku_times > 10)  //调小一点或者大一点
     {
       flag_judge_chuku = 0;
       flag_havechuku = 1;
     }
}

/*******************************************************已经出库*********************************************************************/

}



/*十字路口图像处理*/
void shizi_imageprocess (void)
{
/**********************************************  找拐点 ************************************************************/
    int l_down_flag = 0,r_down_flag =0,l_up_flag=0,r_up_flag=0;
    int i;
    // 找拐点
    if (l_down_flag == 0)
    {
        if (L_black[2] != 188 && L_black[3] != 188 && L_black[4] != 188 )
        {
            for (i = 3; i < 85; i++)  // “ 85 ” 待调
            {
                //下拐点范围45待调
                if (abs(L_black[i] - L_black[i - 1]) < 5 && abs(L_black[i + 1] - L_black[i]) < 5 && L_black[i + 2] - L_black[i + 1] > 0 && i + 1 >= 2 && L_black[i + 1] < 180 && i + 1 < 90)
                {
                    lefty[0] = i + 1;
                    leftx[0] = L_black[i+1];
                    l_down_flag = 1;
                    break;
                }
                else
                {
                    lefty[0] = 2;
                    leftx[0] =188;
                }
            }
        }

    }

    if (r_down_flag == 0)
    {
        if (R_black[2] != 1 && R_black[3] != 1 && R_black[4] != 1)
        {
            for (i = 1; i < 110; i++)
            {
                if (abs(R_black[i] - R_black[i - 1]) < 5 && abs(R_black[i + 1] - R_black[i]) < 5 && R_black[i + 2] - R_black[i + 1] < 0 && i + 1 >= 2 && R_black[i + 1] > 2 && i + 1 < 90)
                {
                    righty[0] = i + 1;
                    rightx[0] = R_black[i+1];

    //                lcd_showuint16(0,5,rightx[0]);
//                    lcd_showuint16(0,6,righty[0]);
                    r_down_flag = 1;

                    break;
                 }
                else
                {
                    righty[0] = 2;
                    rightx[0] = 1;
                }
             }
         }

     }



    if (l_up_flag == 0)
    {
        for (i = 3; i < 85; i++)
        {
            if (L_black[i] - L_black[i - 1] < -3 && abs(L_black[i + 1] - L_black[i]) < 4 && abs(L_black[i + 2] - L_black[i + 1]) < 4 && L_black[i] < 186 && (i > lefty[0]))
            {
                lefty[1] = i;
                leftx[1] = L_black[i];
                l_up_flag = 1;

    //            lcd_showuint16(0,6,leftx[1]);
                break;
            }
        }

    }
    if (r_up_flag == 0)
    {
        for (i = 3; i < 110; i++)
        {
            if (R_black[i] - R_black[i - 1] > 3 && abs(R_black[i + 1] - R_black[i]) < 4 && abs(R_black[i + 2] - R_black[i + 1]) < 4 && R_black[i] > 2 && (i > righty[0]))
            {
                righty[1] = i;
                rightx[1] = R_black[i];
                r_up_flag = 1;
                break;
            }
        }
     }
 /**********************************************  找  拐  点 ************************************************************/


/********************************************** （一）正 入 十 字 *************************************************************************************/
    int a=0,b,c;
    if(lefty[0] < righty[0]) b=righty[0];else b=lefty[0];
    if(lefty[1] > righty[1]) c=righty[1];else c=lefty[1];
    for(i=b+1;i<c-1;i++)
    {
        if(L_black[i] > 180 && R_black[i] < 10)  a++;   //确定两边丢线的行数
    }

   int flagl = 0 ,flagr = 0;
   if ((a >= 5 && (lefty[0] > 2 && lefty[1] > 2) || (righty[0] > 2 && righty[1] > 2) )
           && r_up_flag && l_up_flag && l_down_flag && r_down_flag)
    {
        if (lefty[0] >= 2 && lefty[1] >= 2 )                                    //找左线              入十字前
        {
            flagl = 1;
        }

        if (righty[0] >= 2 && righty[1] >= 2 )                                         //找右线
        {
            flagr = 1;
        }
    }


   if( flagl == 1 &&  flagr == 1)
   {
       flag_shizi = 1;
       buxian(leftx[0],lefty[0],leftx[1],lefty[1]);//补线
       buxian(rightx[0],righty[0],rightx[1],righty[1]);//补线
   }


 /********************************************** （一）正 入 十 字 ************************************************************/


 /********************************************** （二）斜 入 十 字 ************************************************************/
//   //斜入十字，看上方过于地下中线的行数
//   int flagxie = 0;
//   if(flagl == 0 || flagr == 0)
//   {
//   int cntl=0,cntr=0;
//   for (i = 3; i < 110; i++)
//   {
//       if (i < 60)
//       {
//           if ((R_black[i] == 1    &&   L_black[i] < 187    &&   R_black[2] == 1   &&   R_black[3] == 1  &&  R_black[i+1] > L_black[2]  && R_black[i + 2] > L_black[3])
//                   || (R_black[2] == 1 && R_black[3] == 1 && R_black[i] > L_black[10] && R_black[i] > L_black[14] && R_black[i-1] > L_black[12]))
//           {
//               if ((R_black[2] == 1 && R_black[3] == 1 && R_black[i] > L_black[10] && R_black[i] > L_black[14] && R_black[i - 1] > L_black[14] && L_black[2] != 188 && L_black[3] != 188))
//               {
//                   cntl = 18;
//               }
//               cntl++;//左斜-- 看到左边图像
//           }
//
//           else if ((L_black[i] == 188 &&  L_black[2] == 188 && L_black[3] == 188    &&  R_black[i] > 1 && L_black[i+1] < R_black[2] && L_black[i + 2] < R_black[3])/*100待调*/
//                   || (L_black [2] == 1 && L_black[3] == 1 && L_black[i] > R_black[10] && L_black[i] > R_black[14] && L_black[i-1] > R_black[14]))
//           {
//               if (L_black[2] == 1 && L_black[3] == 1 && L_black[i] > R_black[10] && L_black[i] > R_black[14] && L_black[i - 1] > R_black[14] && R_black[2] != 1 && R_black[3] != 1)
//               {
//                   cntr = 18;
//               }
//               cntr++;//右斜
//              // lcd_showuint16(0,6,cntr);
//           }
//       }
//
//
//       else //（i>60）
//       {
//           if ((R_black[2] == 1 && R_black[3] == 1 && R_black[i] > L_black[10] && R_black[i] > L_black[14] && R_black[i - 1] > L_black[14] && L_black[2] != 188 && L_black[3] != 188))
//           {
//               cntl = 18;
//              // lcd_showuint16(0,5,cntl);
//               break;
//           }
//           else if (L_black[2] == 188 && L_black[3] == 188    && L_black[i] < R_black[5] && L_black[i] < R_black[7] && L_black[i - 1] < R_black[7] && R_black[2]!=1 && R_black[3] != 1)
//           {
//               cntr = 18;
//             //  lcd_showuint16(0,6,cntr);
//               break;
//           }
//       }
//
//   }
//
//   if (cntl > 16 && cntr < 16)
//   {
//       cntl = 255;                             //左斜入十字
//       flagxie = 1;
//   }
//   else if (cntr > 16 && cntl < 16)
//   {
//       cntr = 255;                             //右斜入十字
//       flagxie = 1;
//   }
//
//  //lcd_showuint16(0,5,cntl);
//  // lcd_showuint16(0,6,cntr);
//
//   if (cntl == 255)
//   {
//       for (i = 5; i < 110; i++)
//       {
//           if ((R_black[i] - R_black[i - 1] > 15) && (R_black[i] - R_black[i - 2] > 15))
//           {
//               lefty[1] = i;
//               leftx[1] = R_black[i];
//
//
//                   righty[0] = 2;
//                   rightx[0] = 20;//(可调)
//                   righty[1] = i-1;
//                   rightx[1] = R_black[i - 1];
//
////               L_black[i] = R_black[i];
////               R_black[i] = R_black[i - 1];
////               LCenter[i] = R_black[i] / 2 + L_black[i] / 2;
//
//               buxian(leftx[0],lefty[0],leftx[1],lefty[1]);//补线
//               buxian(rightx[0],righty[0],rightx[1],righty[1]);
//               break;
//           }
//       }
//
//   }
//   else if (cntr == 255)//右斜，看到右边图像
//   {
//       for (i = 5; i < 110; i++)
//       {
//           if ((L_black[i] - L_black[i - 1]) < -15 && (L_black[i] - L_black[i - 2]) < -15) //左边界跳变15可调
//           {
//               righty[1] = i;
//               rightx[1] = L_black[i];
//
//
//                   leftx[0] = 168;//（可调）
//                   lefty[0] = 2;
//                   leftx[1] = L_black[i - 1];
//                   lefty[1] = i-1;
//
////               R_black[i] = L_black[i];
////               L_black[i] = L_black[i - 1];
////               LCenter[i] = R_black[i] / 2 + L_black[i] / 2;
//
//               buxian(rightx[0],righty[0],rightx[1],righty[1]);//补线
//               buxian(leftx[0],lefty[0],leftx[1],lefty[1]);
//               break;
//           }
//       }
//
//   }
//   }
   /********************************************** （二）斜 入 十 字 ************************************************************/



   /********************************************** （三） 十 字  中 间 ************************************************************/
   if(flag_shizi == 1 && r_up_flag == 1 && l_up_flag ==1 && l_down_flag ==0 && r_down_flag ==0 /*&& flagxie == 0*/)
   {

       leftx[0] = leftx[1];
       lefty[0] = 2;
       righty[0] = 2;
       rightx[0] = rightx[1];

       buxian(rightx[0],righty[0],rightx[1],righty[1]);//补线
       buxian(leftx[0],lefty[0],leftx[1],lefty[1]);

   }
   /********************************************** （三） 十 字  中 间 ************************************************************/

   //最后一步
   if(lefty[1] < 15 || righty[1] < 15) //十字结束
    {
        flag_shizi = 0;
    }

   get_line();//补线处理完重新获取中线


}



/*环岛图像处理*/
void huandao_L(void)
{
/********************************************** （一）刚 到 环 岛 ************************************************************/
if(flag_ruhuan == 0)
{
    float K1 = (R_black[10]-R_black[40]) * 1.0 / (10-40);
    if(K1>20) K1=20;
    if(K1<-20) K1=-20;
    float K2 = (R_black[40]-R_black[70]) * 1.0 / (40-70);
    if(K2>20) K2=20;
    if(K2<-20) K2=-20;
    float K3 = (R_black[10]-R_black[70]) * 1.0 / (10-70);
    if(K3>20) K3=20;
    if(K3<-20) K3=-20;
 //   lcd_showfloat(0,1,K2-K1,3,3);
//    lcd_showfloat(0,2,K3-K1,3,3);//阈值0.05待测


    if (L_black[2] != 188 && L_black[3] != 188 && L_black[4] != 188 ) //下拐点
    {
        for (int i = 3; i < 115; i++)
        {
            if (abs(L_black[i] - L_black[i - 1]) < 5 && abs(L_black[i + 1] - L_black[i]) < 5
                    && L_black[i + 2] - L_black[i + 1] > 5 && L_black[i + 3] - L_black[i + 1] > 5  && L_black[i + 1] < 180 && i  < 90)
            {
                lefty[0] = i + 1;
                leftx[0] = L_black[i+1];
                break;
            }
            else
            {
                lefty[0] = 2;
                leftx[0] =180;
            }
        }
    }


    for (int i = 10; i <= 115; i++)//中拐点
    {
        if ( (L_black[i - 5] - L_black[i - 1]) > 0 && (L_black[i - 4] - L_black[i - 1]) > 0 && (L_black[i - 3] - L_black[i - 1]) > 0
            && (L_black[i - 5] - L_black[i - 3]) >= 0 && (L_black[i - 4] - L_black[i - 2]) >= 0
            && (L_black[i + 4] - L_black[i - 1]) >= 0 && (L_black[i + 3] - L_black[i - 1]) >= 0 && (L_black[i + 2] - L_black[i - 1]) >= 0
            && i > lefty[0]
            )
        {
            lefty[1] = i - 1;
            leftx[1] = L_black[i - 1];
            break;
        }
        else
        {
           lefty[1] = 2;
           leftx[1] = 180;
        }
    }

   for (int i = 10; i < 116; i++)//上拐点
   {
       if (L_black[i] - L_black[i - 1] < -3 && abs(L_black[i + 1] - L_black[i]) < 4 && abs(L_black[i + 2] - L_black[i + 1]) < 4
           && L_black[i] < 160 && (i > lefty[1])
          )
       {
           lefty[2] = i;
           leftx[2] = L_black[i];
           break;
       }
       else
      {
          lefty[2] = 2;
          leftx[2] = 180;
      }
   }
/****************************************************************************************/

   if (  lefty[0] >2 && lefty[1] >2 && lefty[2] >2
         &&  K1-K2<0.05 && K1-K3<0.05 && K2-K3 < 0.05 &&  K1-K2> -0.05 && K1-K3> -0.05 && K2-K3 > -0.05 //阈值待测
      )
    {
        flag_huandao = 1;
    }


   if(flag_huandao ==1)
   {
//       buxian(leftx[0],lefty[0],leftx[1],lefty[1]);
//       buxian(leftx[1],lefty[1],leftx[2],lefty[2]);
       buxian(leftx[0],lefty[0],leftx[2],lefty[2]);

       if(( lefty[0] < 8  )
          ||(Use_Image[2][180] == 1 && Use_Image[4][180] ==1 && Use_Image[6][180] == 1 && Use_Image[8][180] ==1 && Use_Image[10][180] ==1
               && Use_Image[2][177] == 1 && Use_Image[3][177] ==1 && Use_Image[4][177] == 1 && Use_Image[5][177] ==1   && Use_Image[10][177] ==1 )
          ||( L_black[3] > 175 &&  L_black[6] > 175  && L_black[9] > 175   )
       )//很需要修改，很不稳，过早判2
       {
           flag_huandao = 2;
           flag_ruhuan = 1;
       }
   }

}
/********************************************** （一）刚 到 环 岛 ************************************************************/



///********************************************** （二）入 环 岛 前 ************************************************************/
if( flag_huandao == 2 )
{
    for (int i = 8; i <= 115; i++)//弧拐点
        {
            if ( (L_black[i - 5] - L_black[i - 1]) > 0 && (L_black[i - 4] - L_black[i - 1]) > 0 && (L_black[i - 3] - L_black[i - 1]) > 0
                && (L_black[i - 5] - L_black[i - 4]) >= 0 && (L_black[i - 4] - L_black[i - 3]) >= 0 && (L_black[i - 3] - L_black[i - 2]) >= 0
                && (L_black[i + 2] - L_black[i - 1]) >= 0 && (L_black[i + 1] - L_black[i - 1]) >= 0 && (L_black[i] - L_black[i - 1]) >= 0
                )
            {
                lefty[0] = i - 1;
                leftx[0] = L_black[i - 1];
                break;
            }
            else
            {
               lefty[0] = 2;
               leftx[0] = 188;
            }
        }

    for (int i = 8; i < 116; i++)//上拐点
       {
           if ( L_black[i] - L_black[i - 1] < -3 && abs(L_black[i + 1] - L_black[i]) < 4 && abs(L_black[i + 3] - L_black[i + 1]) < 4
               && L_black[i ] - L_black[i+1] >=0 && L_black[i ] - L_black[i + 3] >= 1
               && L_black[i] < 180 && i > lefty[0]  )
           {
               lefty[1] = i;
               leftx[1] = L_black[i];
               break;
           }
           else
          {
              lefty[1] = 2;
              leftx[1] = 188;
          }
       }

    if( lefty[0] > 8 && lefty[1] > 20 )
    {
        buxian2(leftx[0],lefty[0],leftx[1],lefty[1]);
    }
    else if(lefty[1] > 20)
    {
        buxian2(160,2,leftx[1],lefty[1]);
    }

    if( L_black[3] < 150 &&  L_black[5] < 150 && L_black[7] <150   ) //150 待测
    {
        flag_huandao = 3;
    }

}
///********************************************** （二）入 环 岛  前************************************************************/


///********************************************** （三）入 环 岛  ************************************************************/
if(flag_ruhuan == 1 && flag_huandao == 3)
{
    for (int i = 3; i < 116; i++)//上拐点
  {
      if (L_black[i] - L_black[i - 1] < -3 && abs(L_black[i + 1] - L_black[i]) < 4 && abs(L_black[i + 2] - L_black[i + 1]) < 4
          && L_black[i] < 170 && (i > 30)
         )
      {
          lefty[1] = i;
          leftx[1] = L_black[i];
          break;
      }
      else
     {
         lefty[1] = 115;
         leftx[1] = 185;
     }
  }
   buxian(R_black[1]+1,2,leftx[1],lefty[1]);


}
///********************************************** （三）入 环 岛  ************************************************************/


get_line();
}


/*三叉路口*/
void sancha(void)
{
    if (L_black[2] != 188 && L_black[3] != 188 && L_black[4] != 188 ) //左拐点
          {
              for (int i = 3; i < 115; i++)  // “ 85 ” 待调
              {
                  //下拐点范围45待调
                  if (abs(L_black[i] - L_black[i - 1]) < 5 && abs(L_black[i + 1] - L_black[i]) < 5 && L_black[i + 2] - L_black[i + 1] > 0 && i + 1 >= 2 && L_black[i + 1] < 180 && i + 1 < 90)
                  {
                      lefty[0] = i + 1;
                      leftx[0] = L_black[i+1];
                      break;
                  }
                  else
                  {
                      lefty[0] = 2;
                      leftx[0] =188;
                  }
              }
          }


         if (R_black[2] != 1 && R_black[3] != 1 && R_black[4] != 1) //右拐点
          {
              for (int i = 3; i < 115; i++)
              {
                  if (abs(R_black[i] - R_black[i - 1]) < 5 && abs(R_black[i + 1] - R_black[i]) < 5 && R_black[i + 2] - R_black[i + 1] < 0 && i + 1 >= 2 && R_black[i + 1] > 2 && i + 1 < 90)
                  {
                      righty[0] = i + 1;
                      rightx[0] = R_black[i+1];
                      break;
                   }
                  else
                  {
                      righty[0] = 2;
                      rightx[0] = 1;
                  }
               }
           }

           for(int i=2;i<115;i++)//Y形的判断
           {
               if(LCenter[i+1] - LCenter[i] > 15 || LCenter[i+1] - LCenter[i] < -15)
               {
                   middley[0] = i;
                   middlex[0] = LCenter[i];
                   break;
               }
               else
               {
                   middley[0] = 0;
                   middlex[0] = 0;
               }
            }

   if(middley[0] <80 && middley[0] > 15)
   {
         for(int i= middlex[0]; i<185;i++)      //Y形左边找两点
         {
             if(Use_Image[middley[0]+2][i] == 1 && Use_Image[middley[0]+2][i-1] == 0 && Use_Image[middley[0]+2][i-2] == 0  )
             {
                 middley[1] = middley[0]+2;
                 middlex[1] = i;
                 break;
             }
             else
             {
                 middley[1] = 0;
                 middlex[1] = 0;
             }
         }

         for(int i= middlex[0]; i>10;i--)
          {
              if(Use_Image[middley[0]+6][i] == 1 && Use_Image[middley[0]+6][i+1] == 0 && Use_Image[middley[0]+2][i+6] == 0  )
              {
                  middley[2] = middley[0]+6;
                  middlex[2] = i;
                  break;
              }
              else
              {
                  middley[2] = 0;
                  middlex[2] = 0;
              }
          }

         for(int i= middlex[0]; i<185;i++)      //Y形右边找两点
         {
             if(Use_Image[middley[0]+2][i+1] == 1  && Use_Image[middley[0]+2][i] == 1 && Use_Image[middley[0]+2][i-1] == 0 && Use_Image[middley[0]+2][i-2] == 0  )
             {
                 middley[3] = middley[0]+2;
                 middlex[3] = i;
                 break;
             }
             else
             {
                 middley[3] = 0;
                 middlex[3] = 0;
             }
         }

         for(int i= middlex[0]; i>10;i--)
          {
              if(Use_Image[middley[0]+6][i-1] == 1 && Use_Image[middley[0]+6][i] == 1 && Use_Image[middley[0]+6][i+1] == 0 && Use_Image[middley[0]+2][i+6] == 0  )
              {
                  middley[4] = middley[0]+6;
                  middlex[4] = i;
                  break;
              }
              else
              {
                  middley[4] = 0;
                  middlex[4] = 0;
              }
          }

         if(lefty[0] > 10 && righty[0] >10
             &&   middlex[1] -  middlex[0] < 40 &&  middlex[2] -  middlex[0] < 40 &&  middlex[1] < middlex[2]
             &&   middlex[0] -  middlex[3] < 40 &&  middlex[0] -  middlex[4] < 40 &&  middlex[4] < middlex[3]
            )
         {
             flag_sancha = 1;
         }
  }

}




/*斑马线识别以及入库*/
void check_starting_line(int start_point, int end_point)
{
/********************************************** （一）确 定 斑 马 线  ************************************************************/
    int times = 0;
    flag_starting_line = 0;
    for (int y = start_point; y <= end_point; y++)
    {
        int black_blocks = 0;
        int cursor = 0;    //指向栈顶的游标
        for (int x = 5; x <= 185; x++)
        {
            if (Use_Image[y][x] == 0)//黑点
            {
                if (cursor >= 20)
                {
                    break;          //当黑色元素超过栈长度的操作   break;
                }
                else
                {
                    cursor++;
                }
            }

            else//(白点)
            {
                if (cursor >= 4 && cursor <= 8)//宽度4-8确定为条纹(待调)
                {
                    black_blocks++;

                    for(int i = x;i>=x-cursor-1;i--)//清除条纹!!!  效果不是很好，可以在计算pid时去掉这部分的中值
                    {
                        Bin_Image[119-y][187-i] = 1;
                        Bin_Image[119-y][187-i+1] = 1;
                        Bin_Image[119-y][187-i-1] = 1;
                        Bin_Image[119-y-1][187-i] = 1;
                        Bin_Image[119-y+1][187-i] = 1;
                    }

                    cursor = 0;
                }
                else
                {
                    cursor = 0;
                }
            }

        }

        if (black_blocks >= 6 && black_blocks <= 9) times++;
    }
    if (times >= 8)
    {
        flag_starting_line = 1;
        flag_ruku =1;    //只跑一圈，只要一发现斑马线，就可以入库了
        flag_find_cheku = 0;
    }
    else
    {
        flag_starting_line = 0;
    }
//    lcd_showuint16(0,5,flag_starting_line);
//    lcd_showuint16(0,6,flag_ruku);

/********************************************** （一）确 定 斑 马 线  ************************************************************/



/********************************************** （二）远 处 斑 马 线  ************************************************************/
 int i =0,x=0,y=0,y1=0,x1=0;
 int flag_rukuqian = 0;
 if (flag_starting_line = 1)
 {
        if (R_black[2] != 1 && R_black[3] != 1 && R_black[4] != 1)
        {
            for (i = 1; i < 120; i++)
            {
                if (abs(R_black[i] - R_black[i - 1]) < 5 && abs(R_black[i + 1] - R_black[i]) < 5 && R_black[i + 2] - R_black[i + 1] < 0 && i + 1 >= 2 && R_black[i + 1] > 2 && i + 1 < 90)
                {
                    righty[0] = i + 1;
                    rightx[0] = R_black[i+1];
                    break;
                }
            }
        }
       // lcd_showuint16(0,6,righty[0]);


        if (righty[0] > 2 && righty[0] < 120)
        {
            for (y = righty[0] + 5; y < 120; y++)//(下拐点上5行开始往上找，连续右侧两点黑，5   ***待调***)
            {
                if (Use_Image[y][ 1] == 0 && Use_Image[y + 2][ 1] == 0)
                {
                    y1 = y;
                    break;
                }
            }
           //lcd_showuint16(0,5,y1);
        }

        else//(没找到下拐点)
        {
            for (y = 45; y < 120; y++)
            {
                if (Use_Image[y][ 1] == 0 && Use_Image[y + 2][1] == 0)
                {
                    y1 = y;
                    break;
                }
            }
        }


        for (x = 1; x < 185; x++)
        {
            if (Use_Image[y1][ x] != 0)  //从右到左 找到白点
            {
                x1 = x;
                break;
            }
        }

        int y_zhengque = 0;
        for (y = y1 + 1; y <= y1 + 4; y++)//纠正 确定正确的上拐点
        {
            int X = 0;
            for (x = 1; x < 185; x++)
            {
                if (Use_Image[y][ x] != 0)
                {
                    X = x;
                    break;
                }
            }
            if (X >= x1)
            {
                x1 = X;
                y_zhengque = y;
            }
        }

        y1 = y_zhengque;
        righty[1] = y1;
        rightx[1] = x1;

        if (rightx[0] > rightx[1])         //普通扫线扫到斑马线有时会扫出几个很远的点
        {
            rightx[0] = 0;
            righty[0] = 0;
        }

        if(rightx[0]>20)//远近分界线
        {
            flag_rukuqian = 1;
            buxian(rightx[0],righty[0],rightx[1],righty[1]);//补线并定义边界
        }
        else
        {
            flag_rukuqian = 0;//还需要根据情况修改，条件较少，判定不太稳定
        }
 }
/********************************************** （二）远 处 斑 马 线  ************************************************************/




 /********************************************** （三）近 处 斑 马 线 && 入 库  ************************************************************/
 /*近处发现了斑马线的时候，下拐点就不太重要了，主要就是通过找到右上拐点，与左下边线拉线入库。右上拐点的找寻方法依旧和上边是一样的，然后找左边线第一个不丢线的点作为左下拐点。*/
 if(flag_ruku == 1 && flag_rukuqian ==0)
 {
     if (Use_Image[2][ 188] != 0)   //白

     {

         for (y = 2; y < 80; y++)
         {
             if (Use_Image[y][ 188] == 0)
             {
                 y1 = y;
                 break;
             }
         }
     }
     else
     {
         y1 = 0;
     }

     for (x = 188; x > 2; x--)
     {
         if (Use_Image[y1][ x] != 0)
         {
             x1 = x;
             break;
         }
     }
     lefty[0] = y1;
     leftx[0] = x1;//左下点

     //开始找右上拐点
     if (Use_Image[2][ 1] == 0 && Use_Image[3][ 1] == 0 && Use_Image[4][ 1] == 0 && Use_Image[5][ 1] == 0 && Use_Image[6][ 1] == 0)//连续黑色（右下有黑块）
     {
         for (y = 5; y < 80; y++)
         {
             if (Use_Image[y][ 1] != 0 && y >= 6)//为白（找到右下的白点）
             {
                 y1 = y;
                 righty[0] = y;
                 break;
             }

         }

         for (y = y1; y < 75; y++)
         {
             if (Use_Image[y][ 1] == 0 && y >= 15 && Use_Image[y + 2][ 1] == 0)  //从右下白点往上找黑点
             {
                 y1 = y;
                 break;
             }

         }
     }


     else//（右下全白无黑块直接往上找黑点）
     {
         righty[0] = 2;
         for (y = 10; y < 80; y++)
         {
             if (Use_Image[y][ 1] == 0 && y >= 15 && Use_Image[y + 2][ 1] == 0)
             {
                 y1 = y;
                 break;
             }
         }
     }

     for (x = 1; x < 188; x++)
     {
         if (Use_Image[y1][ x] != 0)
         {
             x1 = x;
             break;
         }
     }

     int y_zhengque = 0;
     for (y = y1 + 1; y <= y1 + 3; y++)
     {
         int X = 0;
         for (x = 0; x < 185; x++)
         {
             if (Use_Image[y][ x] != 0)
             {
                 X = x;
                 break;
             }
         }
         if (X >= x1)
         {
             x1 = X;
             y_zhengque = y;
         }
     }
     y1 = y_zhengque;
     righty[1] = y1;
     rightx[1] = x1;

     for(i=righty[0];i<righty[1];i++)//清除斑马线
     {
         for(int j=1;j<188;j++)
         {
             Bin_Image[119-i][187-j] = 1;
         }
     }

     buxian(leftx[0],lefty[0],rightx[1],righty[1]);//补线
 }



 /********************************************** （三）近 处 斑 马 线 && 入 库  ************************************************************/
 get_line();
}




























