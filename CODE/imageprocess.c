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

void huandao(void)
{
 /********************************************** （一）识 别 环 岛 ************************************************************/

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

   if(flag_ruhuan == 0 && (flag_huandao == 0 || flag_huandao == 1) )
   {
        if (L_black[2] < 180 && L_black[3] < 180 && L_black[4] < 180 ) //下拐点
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

       if (  lefty[0] >2 && lefty[1] >2 && lefty[2] >2  && leftx[0] > leftx[2]
             &&  K1-K2<0.05 && K1-K3<0.05 && K2-K3 < 0.05 &&  K1-K2> -0.05 && K1-K3> -0.05 && K2-K3 > -0.05 //阈值待测
          )
        {
            flag_huandao = 1;
        }

       if(flag_huandao == 1 && lefty[0] >2 && lefty[1] >2 && lefty[2] >2  && leftx[0] > leftx[2] )
       {
           buxian(leftx[0],lefty[0],leftx[2],lefty[2]);
       }

    }
 /********************************************** （一）识 别 环 岛 ************************************************************/

 /***********************************阶段二*************************************************************/
    if(flag_huandao == 1)
    {
        int white = 0;
        for(int i=2;i<40;i++)
        {
            if(L_black[i] > 175 && R_black[i] > 15)
            {
                white++;
            }
        }

        if(white > 30 )
        {
            flag_huandao = 2;
        }

    }

    if(flag_huandao == 2)
    {
        for (int i = 10; i <= 115; i++)//中拐点
       {
           if ( (L_black[i - 4] - L_black[i - 1]) > 1 && (L_black[i - 3] - L_black[i - 1]) > 1
               && (L_black[i - 5] - L_black[i - 3]) > 1 && (L_black[i + 5] - L_black[i + 3]) >1
               && (L_black[i + 4] - L_black[i - 1]) > 1 && (L_black[i + 3] - L_black[i - 1]) > 1
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

       buxian2(leftx[1],lefty[1],leftx[1]+10,2);

       int black = 0;
       for(int i=2;i<30;i++)
       {
           if(L_black[i] < 180 && R_black[i] > 15)
           {
               black++;
           }
       }

       if(black > 20 )
       {
           flag_huandao = 3;
       }

    }
 /*******************************************************************************************************************/

 /***********************************阶段三****************************************************************************/
  if(flag_huandao == 3)
  {
      for (int i = 10; i < 116; i++)//上拐点
     {
         if (L_black[i] - L_black[i - 1] < -4 && abs(L_black[i + 1] - L_black[i]) < 4 && abs(L_black[i + 2] - L_black[i + 1]) < 4
             && L_black[i] < L_black[2]
            )
         {
             lefty[1] = i;
             leftx[1] = L_black[i];
             break;
         }
         else
        {
            lefty[2] = 2;
            leftx[2] = 180;
        }
     }

      buxian(leftx[2],lefty[2],L_black[2],2);

      int white = 0;
      for(int i = 2;i<30;i++)
      {
          if(L_black[i] > 180 && R_black[i] > 15)
          {
              white ++;
          }
      }
      if(white > 20)
      {
          flag_huandao =4;
      }

  }
  /**********************************************************************************************************************/

  /***********************************阶段三****************************************************************************/
  if(flag_huandao == 4 )
  {
      for (int i = 10; i < 116; i++)//上拐点
      {
          if (L_black[i] - L_black[i - 1] < -4
              && abs(L_black[i + 1] - L_black[i]) < 4 && abs(L_black[i + 2] - L_black[i + 1]) < 4
              && L_black[i] < L_black[2]
             )
          {
              lefty[1] = i;
              leftx[1] = L_black[i];
              break;
          }
          else
         {
             lefty[2] = 2;
             leftx[2] = 180;
         }
      }

      buxian(leftx[2],lefty[2],R_black[2],2);

      /**(继续采集图像再说)*********/
      int white = 0;
      for(int i = 2; i<30 ;i++)
      {
          if(R_black[i] < 10)
          {
              white ++;
          }

      }
      if(white > 20)
      {
          flag_huandao = 5;
      }
  }




  /********************************************************************************************************************/






get_line();
}




void  sancha(void)
{
    /*******************************************识别三叉**********************************************/
    if (L_black[2] < 180 && L_black[3] < 180 && L_black[4] < 180  )
       {
           for (int i = 5; i < 85; i++)
           {
               if (abs(L_black[i] - L_black[i - 1]) < 5 && abs(L_black[i + 1] - L_black[i]) < 5
                   && L_black[i + 2] - L_black[i + 1] > 0 && i + 1 >= 2 && L_black[i + 1] < 180
                   && L_black[i + 1] < L_black[2]
                   )
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



        if (R_black[2] >= 10 && R_black[3] >= 10 && R_black[4] >= 10)
       {
           for (int i = 5; i < 85; i++)
           {
               if (abs(R_black[i] - R_black[i - 1]) < 5 && abs(R_black[i + 1] - R_black[i]) < 5
                       && R_black[i + 2] - R_black[i + 1] < 0 && i + 1 >= 2 && R_black[i + 1] > 2  && R_black[i+1] > R_black[2])
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

        for(int i=2;i<110;i++)//Y形的判断
       {
           if(LCenter[i+1] - LCenter[i] > 20 )
           {
               middley[0] = i;
               middlex[0] =  LCenter[i];
               break;
           }
           else
           {
               middley[0] = 2;
               middlex[0] = 90;
           }
        }

        int black = 0;
        for(int i = middley[0];i<middley[0]+20;i++)
        {
            if(Use_Image[i][middlex[0]] == 0)
            {
                black ++;
            }
        }

        if(black > 10 && middley[0] >10 && lefty[0] >2 && righty[0]>2
           && middlex[0] > rightx[0] && middlex[0] < leftx[0] && middley[0] > lefty[0] && middley[0] > righty[0]
          )
        {
            flag_sancha = 1;
        }
 /***************************************************************************************************************/

        if(flag_sancha == 1)
        {
           int white = 0;
           for(int i=2;i<40;i++)
            {
                if(L_black[i] > 175 && R_black[i] < 15)
                {
                    white++;
                }
            }

            if(white > 25 )
            {
                flag_sancha = 2;
            }
         }

        if(flag_sancha ==2)
        {
              for(int i=2;i<110;i++)//Y形的判断
              {
                  if(LCenter[i+1] - LCenter[i] > 20 )
                  {
                      middley[0] = i;
                      middlex[0] =  LCenter[i];
                      break;
                  }
                  else
                  {
                      middley[0] = 2;
                      middlex[0] = 90;
                  }
               }

              buxian(middlex[0],middley[0],R_black[2],2);

                int white = 0;
                for(int i=2;i<40;i++)
                 {
                     if(L_black[i] > 175 && R_black[i] < 15)
                     {
                         white++;
                     }
                 }

              if(middley[0] < 30 || white < 25)//待调
              {
                  flag_sancha = 0;
              }
        }

/*****************************************************************************************************************/








  get_line();

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
        if (L_black[2] < 180 && L_black[3] < 180 && L_black[4] < 180 )
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
        if (R_black[2] >= 10 && R_black[3] >= 10 && R_black[4] >= 10)
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
        if (lefty[0] >= 2 && lefty[1] >= 2 && leftx[0] < 170 && leftx[1] < 170 && leftx[0]>leftx[1])                                    //找左线              入十字前
        {
            flagl = 1;
        }

        if (righty[0] >= 2 && righty[1] >= 2 && rightx[0] > 20 && rightx[1] > 20 && rightx[0] < rightx[1])                                         //找右线
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
