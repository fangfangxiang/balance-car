/**************************************************/


#include "headfile.h"
#pragma section all "cpu0_dsram"
#include "camera.h"
#include "imageprocess.h"
#include"dir.h"

int Vat=0;
int Vbat=0;

extern uint8 Bin_Image[120][188];
extern uint8 LCenter [LCDH];
extern uint8 L_black[LCDH];
extern uint8 R_black[LCDH];

extern int flag_chukechange ;
extern int flag_huandao ;
extern int dutty,encoder1;

//���⣬Ϲд��
sint16 TempAngle = 0;
float gyro[3];// �絥�����ҽ��ٶ�
float   Pitch=6.0;  //�����ǲ�


int core0_main(void)
{
    get_clk();

    //***��ʼ������:***//
    lcd_init();//��Ļ lcd_showuint16(0,6,);
    mt9v03x_init();
    gpt12_init(GPT12_T2, GPT12_T2INB_P33_7, GPT12_T2EUDB_P33_6);  //���ֱ�����
    gpt12_init(GPT12_T4, GPT12_T4INA_P02_8, GPT12_T4EUDA_P00_9);  //�����ֱ�����

    gtm_pwm_init(ATOM0_CH4_P02_4, 17000, 0);//�����ֵ��
    gtm_pwm_init(ATOM0_CH5_P02_5, 17000, 0);
    gtm_pwm_init(ATOM0_CH4_P02_4, 17000, 0);//���ֵ��
    gtm_pwm_init(ATOM0_CH5_P02_5, 17000, 0);
    gtm_pwm_init(ATOM1_CH1_P33_9, 50, 750); //�����ʼ�����Ҿ���


    //�ȴ����к��ĳ�ʼ�����
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts();

    //****�ж�****//
 //   pit_interrupt_ms(CCU6_0, PIT_CH0,10);

while (TRUE)
{
        Get_Bin_Image (0);

        Bin_Image_Filter();
        get_line();

//        if(flag_chukechange == 1)   //ͼ������
//        {
//           // chuku();
//            flag_chukechange =0;
//        }
//        else
//        {
//        shizi_imageprocess ();//ʮ��·��
//        huandao_L();
//        lcd_showint16(0,6,flag_huandao);
//        }
//
        dir_pd();

        /*��ʾ��ֵ��ͼ��*/
       if(mt9v03x_finish_flag)
       {
           for (int i=5;i<IMAGEH ;i++)
            {
                 for(int j=0;j<IMAGEW;j++)
                 {
                      if(Bin_Image[i][j])
                          mt9v03x_image[i][j]=255;
                      else
                          mt9v03x_image[i][j]=0;
                 }
               mt9v03x_image[i][188-LCenter[120-i]]=0;
               mt9v03x_image[i][188-L_black[120-i]]=0;
               mt9v03x_image[i][188-R_black[120-i]]=0;
             }

        lcd_displayimage032(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
        mt9v03x_finish_flag = 0;
       }


       encoder1 = gpt12_get(GPT12_T2);
       gpt12_clear(GPT12_T2);
      // lcd_showint16(0,1,encoder1);

       dutty = SBB_Get_MotorPI (encoder1 , 800 );
     //  lcd_showint16(0,3,dutty);


        if(dutty >= 0)
         {
             pwm_duty(ATOM0_CH4_P02_4, dutty);
             pwm_duty(ATOM0_CH5_P02_5, 0);
         }
         else
         {
             pwm_duty(ATOM0_CH4_P02_4, 0);
             pwm_duty(ATOM0_CH5_P02_5, -dutty);
         }

       systick_delay_ms(STM0, 40);
}

}


#pragma section all restore


