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

//待测，瞎写的
sint16 TempAngle = 0;
float gyro[3];// 电单车左右角速度
float   Pitch=6.0;  //陀螺仪测


int core0_main(void)
{
    get_clk();

    //***初始化函数:***//
    lcd_init();//屏幕 lcd_showuint16(0,6,);
    mt9v03x_init();
    gpt12_init(GPT12_T2, GPT12_T2INB_P33_7, GPT12_T2EUDB_P33_6);  //后轮编码器
    gpt12_init(GPT12_T4, GPT12_T4INA_P02_8, GPT12_T4EUDA_P00_9);  //动量轮编码器

    gtm_pwm_init(ATOM0_CH4_P02_4, 17000, 0);//动量轮电机
    gtm_pwm_init(ATOM0_CH5_P02_5, 17000, 0);
    gtm_pwm_init(ATOM0_CH4_P02_4, 17000, 0);//后轮电机
    gtm_pwm_init(ATOM0_CH5_P02_5, 17000, 0);
    gtm_pwm_init(ATOM1_CH1_P33_9, 50, 750); //舵机初始化并且居中


    //等待所有核心初始化完毕
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts();

    //****中断****//
 //   pit_interrupt_ms(CCU6_0, PIT_CH0,10);

while (TRUE)
{
        Get_Bin_Image (0);

        Bin_Image_Filter();
        get_line();

//        if(flag_chukechange == 1)   //图像处理部分
//        {
//           // chuku();
//            flag_chukechange =0;
//        }
//        else
//        {
//        shizi_imageprocess ();//十字路口
//        huandao_L();
//        lcd_showint16(0,6,flag_huandao);
//        }
//
        dir_pd();

        /*显示二值化图像*/
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


