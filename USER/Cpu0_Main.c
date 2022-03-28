/**************************************************/


#include "headfile.h"
#pragma section all "cpu0_dsram"
#include "camera.h"
#include "imageprocess.h"
#include"dir.h"
#include"ui.h"
#include "tuoluoyi.h"
#include "Balance1.h"

int Vat=0;
int Vbat=0;

extern uint8 Bin_Image[120][188];
extern uint8 LCenter [LCDH];
extern uint8 L_black[LCDH];
extern uint8 R_black[LCDH];

extern int flag_havechuku ;
extern int flag_huandao ;
extern int dutty ,encoder1,vtarget;

//���⣬Ϲд��
sint16 TempAngle = 0;
float gyro[3];// �絥�����ҽ��ٶ�
float   Pitch=6.0;  //�����ǲ�

uint8 uart_buff;
int flag_break = 0;

int core0_main(void)
{
    get_clk();

    //***��ʼ������:***//
    lcd_init();//��Ļ lcd_showuint16(0,6,);
   // mt9v03x_init();
   // icm20602_init_spi();//icm20602��ʼ��
    gpt12_init(GPT12_T2, GPT12_T2INB_P33_7, GPT12_T2EUDB_P33_6);  //���ֱ�����
    gpt12_init(GPT12_T4, GPT12_T4INA_P02_8, GPT12_T4EUDA_P00_9);  //�����ֱ�����

    gtm_pwm_init(ATOM0_CH4_P02_4, 17000, 0);//�����ֵ��
    gtm_pwm_init(ATOM0_CH5_P02_5, 17000, 0);
    gtm_pwm_init(ATOM0_CH6_P02_6, 17000, 0);//���ֵ��
    gtm_pwm_init(ATOM0_CH7_P02_7, 17000, 0);
    gtm_pwm_init(ATOM1_CH1_P33_9, 50, 750); //�����ʼ�����Ҿ���

    uart_init(UART_1, 9600, UART1_TX_P11_12, UART1_RX_P11_10); //����
    UI_Init();


    //�ȴ����к��ĳ�ʼ�����
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts();

    //****�ж�****//
   // pit_interrupt_ms(CCU6_0, PIT_CH0, 10); //ƽ��������ȼ�
    pit_interrupt_ms(CCU6_0, PIT_CH1, 40);  //ͼ��������
   // pit_interrupt_ms(CCU6_1, PIT_CH0, 40);   //�������


while (1)//��ʾ�������ȼ����
{
       UI_Main();

        /*��ʾ��ֵ��ͼ��*/
//       if(mt9v03x_finish_flag)
//       {
//           for (int i=5;i<IMAGEH ;i++)
//            {
//                 for(int j=0;j<IMAGEW;j++)
//                 {
//                      if(Bin_Image[i][j])
//                          mt9v03x_image[i][j]=255;
//                      else
//                          mt9v03x_image[i][j]=0;
//                 }
//               mt9v03x_image[i][188-LCenter[120-i]]=0;
//               mt9v03x_image[i][188-L_black[120-i]]=0;
//               mt9v03x_image[i][188-R_black[120-i]]=0;
//             }
//
//        lcd_displayimage032(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
//        mt9v03x_finish_flag = 0;
//       }

       systick_delay_ms(STM0, 40);
}

}


#pragma section all restore


