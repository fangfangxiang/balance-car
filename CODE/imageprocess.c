/*
 * imageprocess.c
 *
 *  Created on: 2022��1��27��
 *      Author: ���õ����ñ�
 */

#include"headfile.h"
#include "imageprocess.h"
#include "camera.h"

#define LCDH  120  /*!< ����ͷ�ɼ��߶� */
#define LCDW  188   /*!< ����ͷ�ɼ���� */
extern float TempAngle;

extern uint8 Bin_Image[120][188];
extern uint8 Use_Image[120][188];
extern uint8 L_black[LCDH];
extern uint8 R_black[LCDH];
extern uint8 LCenter [LCDH];

int leftx[3],lefty[3],rightx[3],righty[3];//�����¹յ�
int middle[4];//ͻ���
int middlex[4],middley[4];


int chuku_times = 0;
int flag_cont = 0;//����ǵ����㣬���ظ�ʹ��


/********************************��־**********************************************/
int flag_judge_chuku = 0;
int flag_havechuku = 0;

int flag_shizi = 0;
int flag_sancha = 0;

int flag_huandao = 0;//�����Ǹ��׶ξ͸ĳɼ����뻷�ͳ�������1
int flag_ruhuan = 0;
int flag_chuhuan = 0;
int flag_rusancha = 0;

int flag_starting_line = 0;
int flag_ruku = 0;
int flag_find_cheku ;
/************************************************************************************/



/*���������ҹ�*//**********************************************************************************************************/
void chuku(void)
{
    int y , x;
    int y1 = 2;
    int x1 =3;
    righty[0] = 2;

    if (R_black[2] != 1 && R_black[3] != 1 && R_black[4] != 1)//���ҹյ�
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

/*****************************************************���߽׶�**********************************************************/
if(righty[0] < 30)//Ҫ��   //�ж��ҹյ�λ�ã�������йյ��Ϸ���ɫy1
{

    if (Use_Image[10][3] != 0)// ��  �յ��ѽ���������
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
    else //��
    {
        int cnt = 0;
        for (y = 4; y < 110; y++)
        {
            if (Use_Image[y][ 3] != 0 && Use_Image[y - 1][ 3] == 0) //�ϰ��º�
            {
                cnt = 1;
            }

            if (Use_Image[y][ 3] == 0 && Use_Image[y - 1][ 3] != 0 && cnt == 1) //�Ϻ��°�--y1
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


    for (x = 3; x < 185; x++)//��ȡ�������ߵ�
    {
        if (Use_Image[y1][ x] != 0)//��  �ϱߺ�����б
        {
            righty[1] = y1;
            rightx[1] = x;
            break;
        }

        if (x > 93)//�Ϻڱ߽���б
        {
            righty[1] = y1;
            rightx[1] = 0;
            break;
        }
    }


    for (y = 5; y < 70; y++)//  ������±����ߵ�
    {
        if (Use_Image[y][ 185] != 0) //��
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

    for(int i=righty[0];i<righty[1];i++)//���������
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
/*****************************************************���߽׶�***************************************************/

/************************************************�ж��ѳ���*****************************************************************/
if(flag_judge_chuku == 1)
{
    int out = 0;  //����ɹ��жϣ������ã�Ӧ���Ǹ�����ʱȫ��Ϊ��//Ҫ�����߸ķ���
    for( int i=110;i>=30;i--)
    {
        if(Bin_Image[i][10] ==0  && Bin_Image[i][180] ==0) //����Ϊ��
        {
            out ++;
        }
    }

    if(out > 60)
   {
    chuku_times++;
    }

    if(chuku_times > 10)  //��Сһ����ߴ�һ��
     {
       flag_judge_chuku = 0;
       flag_havechuku = 1;
     }
}

/*******************************************************�Ѿ�����*********************************************************************/

}



/*ʮ��·��ͼ����*/
void shizi_imageprocess (void)
{
/**********************************************  �ҹյ� ************************************************************/
    int l_down_flag = 0,r_down_flag =0,l_up_flag=0,r_up_flag=0;
    int i;
    // �ҹյ�
    if (l_down_flag == 0)
    {
        if (L_black[2] != 188 && L_black[3] != 188 && L_black[4] != 188 )
        {
            for (i = 3; i < 85; i++)  // �� 85 �� ����
            {
                //�¹յ㷶Χ45����
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
 /**********************************************  ��  ��  �� ************************************************************/


/********************************************** ��һ���� �� ʮ �� *************************************************************************************/
    int a=0,b,c;
    if(lefty[0] < righty[0]) b=righty[0];else b=lefty[0];
    if(lefty[1] > righty[1]) c=righty[1];else c=lefty[1];
    for(i=b+1;i<c-1;i++)
    {
        if(L_black[i] > 180 && R_black[i] < 10)  a++;   //ȷ�����߶��ߵ�����
    }

   int flagl = 0 ,flagr = 0;
   if ((a >= 5 && (lefty[0] > 2 && lefty[1] > 2) || (righty[0] > 2 && righty[1] > 2) )
           && r_up_flag && l_up_flag && l_down_flag && r_down_flag)
    {
        if (lefty[0] >= 2 && lefty[1] >= 2 )                                    //������              ��ʮ��ǰ
        {
            flagl = 1;
        }

        if (righty[0] >= 2 && righty[1] >= 2 )                                         //������
        {
            flagr = 1;
        }
    }


   if( flagl == 1 &&  flagr == 1)
   {
       flag_shizi = 1;
       buxian(leftx[0],lefty[0],leftx[1],lefty[1]);//����
       buxian(rightx[0],righty[0],rightx[1],righty[1]);//����
   }


 /********************************************** ��һ���� �� ʮ �� ************************************************************/


 /********************************************** ������б �� ʮ �� ************************************************************/
//   //б��ʮ�֣����Ϸ����ڵ������ߵ�����
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
//               cntl++;//��б-- �������ͼ��
//           }
//
//           else if ((L_black[i] == 188 &&  L_black[2] == 188 && L_black[3] == 188    &&  R_black[i] > 1 && L_black[i+1] < R_black[2] && L_black[i + 2] < R_black[3])/*100����*/
//                   || (L_black [2] == 1 && L_black[3] == 1 && L_black[i] > R_black[10] && L_black[i] > R_black[14] && L_black[i-1] > R_black[14]))
//           {
//               if (L_black[2] == 1 && L_black[3] == 1 && L_black[i] > R_black[10] && L_black[i] > R_black[14] && L_black[i - 1] > R_black[14] && R_black[2] != 1 && R_black[3] != 1)
//               {
//                   cntr = 18;
//               }
//               cntr++;//��б
//              // lcd_showuint16(0,6,cntr);
//           }
//       }
//
//
//       else //��i>60��
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
//       cntl = 255;                             //��б��ʮ��
//       flagxie = 1;
//   }
//   else if (cntr > 16 && cntl < 16)
//   {
//       cntr = 255;                             //��б��ʮ��
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
//                   rightx[0] = 20;//(�ɵ�)
//                   righty[1] = i-1;
//                   rightx[1] = R_black[i - 1];
//
////               L_black[i] = R_black[i];
////               R_black[i] = R_black[i - 1];
////               LCenter[i] = R_black[i] / 2 + L_black[i] / 2;
//
//               buxian(leftx[0],lefty[0],leftx[1],lefty[1]);//����
//               buxian(rightx[0],righty[0],rightx[1],righty[1]);
//               break;
//           }
//       }
//
//   }
//   else if (cntr == 255)//��б�������ұ�ͼ��
//   {
//       for (i = 5; i < 110; i++)
//       {
//           if ((L_black[i] - L_black[i - 1]) < -15 && (L_black[i] - L_black[i - 2]) < -15) //��߽�����15�ɵ�
//           {
//               righty[1] = i;
//               rightx[1] = L_black[i];
//
//
//                   leftx[0] = 168;//���ɵ���
//                   lefty[0] = 2;
//                   leftx[1] = L_black[i - 1];
//                   lefty[1] = i-1;
//
////               R_black[i] = L_black[i];
////               L_black[i] = L_black[i - 1];
////               LCenter[i] = R_black[i] / 2 + L_black[i] / 2;
//
//               buxian(rightx[0],righty[0],rightx[1],righty[1]);//����
//               buxian(leftx[0],lefty[0],leftx[1],lefty[1]);
//               break;
//           }
//       }
//
//   }
//   }
   /********************************************** ������б �� ʮ �� ************************************************************/



   /********************************************** ������ ʮ ��  �� �� ************************************************************/
   if(flag_shizi == 1 && r_up_flag == 1 && l_up_flag ==1 && l_down_flag ==0 && r_down_flag ==0 /*&& flagxie == 0*/)
   {

       leftx[0] = leftx[1];
       lefty[0] = 2;
       righty[0] = 2;
       rightx[0] = rightx[1];

       buxian(rightx[0],righty[0],rightx[1],righty[1]);//����
       buxian(leftx[0],lefty[0],leftx[1],lefty[1]);

   }
   /********************************************** ������ ʮ ��  �� �� ************************************************************/

   //���һ��
   if(lefty[1] < 15 || righty[1] < 15) //ʮ�ֽ���
    {
        flag_shizi = 0;
    }

   get_line();//���ߴ��������»�ȡ����


}



/*����ͼ����*/
void huandao_L(void)
{
/********************************************** ��һ���� �� �� �� ************************************************************/
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
//    lcd_showfloat(0,2,K3-K1,3,3);//��ֵ0.05����


    if (L_black[2] != 188 && L_black[3] != 188 && L_black[4] != 188 ) //�¹յ�
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


    for (int i = 10; i <= 115; i++)//�йյ�
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

   for (int i = 10; i < 116; i++)//�Ϲյ�
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
         &&  K1-K2<0.05 && K1-K3<0.05 && K2-K3 < 0.05 &&  K1-K2> -0.05 && K1-K3> -0.05 && K2-K3 > -0.05 //��ֵ����
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
       )//����Ҫ�޸ģ��ܲ��ȣ�������2
       {
           flag_huandao = 2;
           flag_ruhuan = 1;
       }
   }

}
/********************************************** ��һ���� �� �� �� ************************************************************/



///********************************************** �������� �� �� ǰ ************************************************************/
if( flag_huandao == 2 )
{
    for (int i = 8; i <= 115; i++)//���յ�
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

    for (int i = 8; i < 116; i++)//�Ϲյ�
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

    if( L_black[3] < 150 &&  L_black[5] < 150 && L_black[7] <150   ) //150 ����
    {
        flag_huandao = 3;
    }

}
///********************************************** �������� �� ��  ǰ************************************************************/


///********************************************** �������� �� ��  ************************************************************/
if(flag_ruhuan == 1 && flag_huandao == 3)
{
    for (int i = 3; i < 116; i++)//�Ϲյ�
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
///********************************************** �������� �� ��  ************************************************************/


get_line();
}


/*����·��*/
void sancha(void)
{
    if (L_black[2] != 188 && L_black[3] != 188 && L_black[4] != 188 ) //��յ�
          {
              for (int i = 3; i < 115; i++)  // �� 85 �� ����
              {
                  //�¹յ㷶Χ45����
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


         if (R_black[2] != 1 && R_black[3] != 1 && R_black[4] != 1) //�ҹյ�
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

           for(int i=2;i<115;i++)//Y�ε��ж�
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
         for(int i= middlex[0]; i<185;i++)      //Y�����������
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

         for(int i= middlex[0]; i<185;i++)      //Y���ұ�������
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




/*������ʶ���Լ����*/
void check_starting_line(int start_point, int end_point)
{
/********************************************** ��һ��ȷ �� �� �� ��  ************************************************************/
    int times = 0;
    flag_starting_line = 0;
    for (int y = start_point; y <= end_point; y++)
    {
        int black_blocks = 0;
        int cursor = 0;    //ָ��ջ�����α�
        for (int x = 5; x <= 185; x++)
        {
            if (Use_Image[y][x] == 0)//�ڵ�
            {
                if (cursor >= 20)
                {
                    break;          //����ɫԪ�س���ջ���ȵĲ���   break;
                }
                else
                {
                    cursor++;
                }
            }

            else//(�׵�)
            {
                if (cursor >= 4 && cursor <= 8)//���4-8ȷ��Ϊ����(����)
                {
                    black_blocks++;

                    for(int i = x;i>=x-cursor-1;i--)//�������!!!  Ч�����Ǻܺã������ڼ���pidʱȥ���ⲿ�ֵ���ֵ
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
        flag_ruku =1;    //ֻ��һȦ��ֻҪһ���ְ����ߣ��Ϳ��������
        flag_find_cheku = 0;
    }
    else
    {
        flag_starting_line = 0;
    }
//    lcd_showuint16(0,5,flag_starting_line);
//    lcd_showuint16(0,6,flag_ruku);

/********************************************** ��һ��ȷ �� �� �� ��  ************************************************************/



/********************************************** ������Զ �� �� �� ��  ************************************************************/
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
            for (y = righty[0] + 5; y < 120; y++)//(�¹յ���5�п�ʼ�����ң������Ҳ�����ڣ�5   ***����***)
            {
                if (Use_Image[y][ 1] == 0 && Use_Image[y + 2][ 1] == 0)
                {
                    y1 = y;
                    break;
                }
            }
           //lcd_showuint16(0,5,y1);
        }

        else//(û�ҵ��¹յ�)
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
            if (Use_Image[y1][ x] != 0)  //���ҵ��� �ҵ��׵�
            {
                x1 = x;
                break;
            }
        }

        int y_zhengque = 0;
        for (y = y1 + 1; y <= y1 + 4; y++)//���� ȷ����ȷ���Ϲյ�
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

        if (rightx[0] > rightx[1])         //��ͨɨ��ɨ����������ʱ��ɨ��������Զ�ĵ�
        {
            rightx[0] = 0;
            righty[0] = 0;
        }

        if(rightx[0]>20)//Զ���ֽ���
        {
            flag_rukuqian = 1;
            buxian(rightx[0],righty[0],rightx[1],righty[1]);//���߲�����߽�
        }
        else
        {
            flag_rukuqian = 0;//����Ҫ��������޸ģ��������٣��ж���̫�ȶ�
        }
 }
/********************************************** ������Զ �� �� �� ��  ************************************************************/




 /********************************************** �������� �� �� �� �� && �� ��  ************************************************************/
 /*���������˰����ߵ�ʱ���¹յ�Ͳ�̫��Ҫ�ˣ���Ҫ����ͨ���ҵ����Ϲյ㣬�����±���������⡣���Ϲյ����Ѱ�������ɺ��ϱ���һ���ģ�Ȼ��������ߵ�һ�������ߵĵ���Ϊ���¹յ㡣*/
 if(flag_ruku == 1 && flag_rukuqian ==0)
 {
     if (Use_Image[2][ 188] != 0)   //��

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
     leftx[0] = x1;//���µ�

     //��ʼ�����Ϲյ�
     if (Use_Image[2][ 1] == 0 && Use_Image[3][ 1] == 0 && Use_Image[4][ 1] == 0 && Use_Image[5][ 1] == 0 && Use_Image[6][ 1] == 0)//������ɫ�������кڿ飩
     {
         for (y = 5; y < 80; y++)
         {
             if (Use_Image[y][ 1] != 0 && y >= 6)//Ϊ�ף��ҵ����µİ׵㣩
             {
                 y1 = y;
                 righty[0] = y;
                 break;
             }

         }

         for (y = y1; y < 75; y++)
         {
             if (Use_Image[y][ 1] == 0 && y >= 15 && Use_Image[y + 2][ 1] == 0)  //�����°׵������Һڵ�
             {
                 y1 = y;
                 break;
             }

         }
     }


     else//������ȫ���޺ڿ�ֱ�������Һڵ㣩
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

     for(i=righty[0];i<righty[1];i++)//���������
     {
         for(int j=1;j<188;j++)
         {
             Bin_Image[119-i][187-j] = 1;
         }
     }

     buxian(leftx[0],lefty[0],rightx[1],righty[1]);//����
 }



 /********************************************** �������� �� �� �� �� && �� ��  ************************************************************/
 get_line();
}




























