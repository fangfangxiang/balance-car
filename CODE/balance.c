/*
 * balance.c
 *
 *  Created on: 2022��1��28��
 *      Author: ���õ����ñ�
 */
#include"balance.h"
#include"camera.h"
#include"headfile.h"
#define Servo_Delta 150            //�������ת���Ĳ�ֵ�������ͺţ����˺Ͷ���۳��й�
#define Servo_Center_Mid 600      //���ֱ����ֵ��
#define Servo_Left_Max   (Servo_Center_Mid+Servo_Delta)      //�����ת����ֵ
#define Servo_Right_Min  (Servo_Center_Mid-Servo_Delta)      //�����ת����ֵ����ֵ��������÷�ʽ�йأ���ʽ

uint8  Start_Flag=0;                    //������־

float X_Balance_KP=4000.001,X_Balance_KI=4.0001,X_Balance_KD=60.001;  // ƽ�⻷PID  ***Pitch��ǶȻ�****PID���� ������**********111111111111��������������Ҫ����
float X_Velocity_KP=90,X_Velocity_KI=90;//***�����ֵ���ٶȻ�***PI����*******222222222222222��������������������Ҫ����

float Pitch_Zero=6.9,  Pitch_error=0.00;//����Pitch��Ƕ����*******��Ҫ����
float Zero_error = 0.00;              // Pitchƫ��ֵ
int PWM_X,PWM_accel;                  // PWM�м���

sint16 MotorDutyA = 0;                // ���ֵ������ռ�ձ���ֵ
sint16 MotorDutyB = 0;                // ���ֵ������ռ�ձ���ֵ

#define Balance_Kp  32                // ���PID����
#define Balance_Kd   0.03             // ���PID����
#define Balance_Ki   0.0020           // ���PID����**********333333333333��������������Ҫ����

//#define Motor_Kp     12               // ���ֵ��PID����
//#define Motor_Ki     20               // ���ֵ��PID����***********44444444444444������������Ҫ����

short  Velocity = 30;               // �ٶȣ���ʱ������Ϊ60�����壬512������  //�����������ֵ��������
short  PWMMotor, PWMServo;            // ������PMW������ֵ
int  Motor_Bias, Motor_Last_Bias, Motor_Integration; // ������ò���
float   Integration;//���pid�Ļ���ֵ��ͣ����
int  Servo_PWM;


unsigned char Flag_Stop = 0, Flag_Show, Flag_Imu;          // ͣ������ʾ��IMU��ɱ�־λ
extern int Vat;
extern int Vbat;

                        extern  sint16 TempAngle;
                        extern float gyro[3];// �絥�����ҽ��ٶ�
extern sint16 OFFSET0;      //��Զ������������ֵ�ۺ�ƫ����
extern sint16 OFFSET1;      //�ڶ���
extern sint16 OFFSET2;      //�����������
extern sint16 TXV;          //���ε���߶ȣ��Ҹ߶�

                        extern float   Pitch;
extern volatile sint16 ECPULSE1;          //�ٶ�ȫ�ֱ���
extern volatile sint16 ECPULSE2;          //�ٶ�ȫ�ֱ���
extern volatile sint32 RAllPulse;         //�ٶ�ȫ�ֱ���



//��Ҫ������
void Balance(void)
{
    int  Servo_PWM;                             // ���PID

    /* ��ȡ������ֵ */
    ECPULSE1 = gpt12_get(GPT12_T2);
    gpt12_clear(GPT12_T2);// �����ַ���  ������1
    ECPULSE2 = gpt12_get(GPT12_T4);
    gpt12_clear(GPT12_T2);// ���ַ���     ������2

   // DMP_Read();//pitch ������
    Seek_Road();  // ��ȡ������ɫƫ��
    TempAngle = OFFSET2+OFFSET1+OFFSET1;    // ����������ɫƫ��ֵ
    Zero_error = Pitch_Zero+TempAngle/800;  // ������б�Ƕ�ƫ��ֵ


    /////// �����ֿ���//////////
    PWM_X = X_balance_Control(Pitch,Pitch_Zero,gyro[0]);// �����ֵ�������������***�ǶȻ�
    PWM_accel = - Velocity_Control(-ECPULSE1);          // �����ֵ��*****�ٶȻ�******������
    MotorDutyA = -(PWM_X-PWM_accel);        //****************(f ���pwm = �ǶȻ�pid - ����ٶȻ�pi)
    /**/
    if(MotorDutyA>8000) MotorDutyA=8000;        // �����ֵ���޷�
    else if(MotorDutyA<-8000) MotorDutyA=-8000; // �����ֵ���޷�
    else if(MotorDutyA<-0) MotorDutyA -=800;    // ����
    else if(MotorDutyA>0) MotorDutyA+=800;      // ����
    if((MotorDutyA<1000)&&(MotorDutyA>-1000))
            MotorDutyA=0;
    if(0<=MotorDutyA) //�����ֵ��3   ��ת//����Ҫ��******��������
     {
         pwm_duty(ATOM0_CH4_P02_4, MotorDutyA);
         pwm_duty(ATOM0_CH5_P02_5, 0);
     }
     else                //���3   ��ת
     {
         pwm_duty(ATOM0_CH4_P02_4, 0);
         pwm_duty(ATOM0_CH5_P02_5, -MotorDutyA);
     }


    ///////// ����������///////
    Servo_PWM = SBB_Get_BalancePID(Pitch, gyro[0]);               // PID���㵥��ƽ���PWM��ֵ
    if( Servo_PWM < - Servo_Delta)    Servo_PWM = - Servo_Delta;  // ����Ƕ�����
    else if(Servo_PWM > Servo_Delta)  Servo_PWM =   Servo_Delta;  // ����Ƕ�����
    PWMServo = Servo_Center_Mid + Servo_PWM;                      // ת��Ϊ�������PWM

    MotorDutyB = SBB_Get_MotorPI(ECPULSE2, Velocity)/5;           // ���ֵ������ʽPI����****�ٶȻ�***

    if((Pitch > 20) || (Pitch < -20))       // ˤ��ͣ���ж�
       Flag_Stop = 1;
//    if(Vbat<2500) Flag_Stop = 1;            // ��ѹ����Լ11V��ֹͣ�������ֹ��ع���
//    else Flag_Stop = 0;

    if(Flag_Stop == 1)                       // ͣ��
    {
        MotorDutyA=0;
        MotorDutyB = 0;                      // ����ر�
        Integration = 0;                     // ���ֲ�������
    }


    //***//
    /*�������*///����Ҫ��*******��������
    pwm_duty(ATOM1_CH1_P33_9,PWMServo);
    /*���ֵ������*///����Ҫ��
    if(0<=MotorDutyB) //���ֵ��   ��ת*******��������
     {
         pwm_duty(ATOM0_CH4_P02_4, MotorDutyB);
         pwm_duty(ATOM0_CH5_P02_5, 0);
     }
     else                //���ֵ��   ��ת
     {
         pwm_duty(ATOM0_CH4_P02_4, 0);
         pwm_duty(ATOM0_CH5_P02_5, -MotorDutyB);
     }

}



/*X��ƽ��PID����,�ǶȻ�********111***********/
float X_balance_Control(float Angle,float Angle_Zero,float Gyro)
{
     float PWM,Bias;
     static float error;
     Bias=Angle-Pitch_Zero;  //��ȡƫ��


     error+=Bias;                                                      //ƫ���ۻ�
     if(error>+30) error=+30;                                          //�����޷�
     if(error<-30) error=-30;                                          //�����޷�


     PWM=X_Balance_KP * Bias + X_Balance_KI * error + X_Balance_KD * Gyro;   //��ȡ������ֵ
     return PWM;
}



/*�������ٶ�PI����,�ٶ���������****222******/
float Velocity_Control(int encoder)
{
    static float Encoder,Encoder_Integral;
    float Velocity,Encoder_Least;

    Encoder_Least = encoder;                                  //�ٶ��˲�
    Encoder *= 0.7;                                           //һ�׵�ͨ�˲���
    Encoder += Encoder_Least*0.3;                             //һ�׵�ͨ�˲���

    Encoder_Integral += Encoder;                              //���ֳ�λ��
    if(Encoder_Integral > +2000) Encoder_Integral = +2000;    //�����޷�
    if(Encoder_Integral < -2000) Encoder_Integral = -2000;    //�����޷�

    Velocity = Encoder * X_Velocity_KP + Encoder_Integral * X_Velocity_KI/100;
                                                              //��ȡ������ֵ
    if(Start_Flag==0) Encoder_Integral=0,Encoder=0,Velocity=0;//ֹͣʱ��������
      return Velocity;
}




/*�����ǿ���PID*********333*******
������ֵ��float Angle,�絥���������
������ֵ��float Gyro �絥�����ҽ��ٶ�
������ֵ��С��ƽ�⣬���ת��PID*/
int SBB_Get_BalancePID(float Angle,float Gyro)
{
    float  Bias;
    int SBB_BalancePID;


    Bias = Angle - Zero_error;     // ���ƽ��ĽǶ���ֵ�ʹ�ʱ����ǵ�ƫ��
    Integration += Bias;           // ����
    if(Integration<-380)      Integration=-380; //�޷�
    else if(Integration>380)  Integration= 380; //�޷�


    //===����ƽ����ƵĶ��PWM  PID���� kp��Pϵ�� kiʽIϵ�� kd��Dϵ��
    SBB_BalancePID = Balance_Kp * Bias + Balance_Ki*Integration + Balance_Kd*Gyro;
    return SBB_BalancePID;
}


//
//#define Motor_Kp     1.05             // ���ֵ��PID����
//#define Motor_Ki     2.1               // ���ֵ��PID����***********44444444444444������������Ҫ����


#define Motor_Kp   4.2             // ���ֵ��PID����
#define Motor_Ki   0.01               // ���ֵ��PID����***********44444444444444������������Ҫ����
#define Motor_Kd   0.01


int32 integral;
/*���ֵ����������ʽPI******444*********/
int SBB_Get_MotorPI (uint16 Encoder,uint16 Target)
{
    static int Pwm ;
    Motor_Bias = Target - Encoder;
    integral += Motor_Bias;
    lcd_showint16(0,4, integral);

    if((Motor_Ki * integral) <- 1000)      integral= -1000 / Motor_Ki; //�޷�
    else if((Motor_Ki * integral) > 1500)  integral =  1500 / Motor_Ki; //�޷�

    Pwm = Motor_Kp * Motor_Bias + Motor_Ki * integral + Motor_Kd * (Motor_Bias - Motor_Last_Bias);
    lcd_showint16(0,5, Pwm);
    //==λ��ʽPID
    Motor_Last_Bias = Motor_Bias;


    if(Pwm > 5000) Pwm = 5000;               // �޷�
    else if(Pwm < 0) Pwm = 0;         // �޷�
    return Pwm;

//    static int Pwm ;
////    Motor_Bias = Encoder - Target;            // ����ƫ��
//
//    Motor_Bias = Target - Encoder;
////    lcd_showint16(0,0, Encoder);
////    lcd_showint16(0,1,Motor_Bias);

//    Pwm += Motor_Kp * (Motor_Bias - Motor_Last_Bias) + Motor_Ki * Motor_Bias;
//    // ==����ʽPI������
//    if(Pwm > 15000) Pwm = 15000;               // �޷�
//    else if(Pwm < -6000) Pwm = -6000;         // �޷�
//    Motor_Last_Bias = Motor_Bias;            // ������һ��ƫ��
//    return Pwm/3;                              // �������
//
}





