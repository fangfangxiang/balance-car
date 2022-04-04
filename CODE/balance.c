/*
 * balance.c
 *
 *  Created on: 2022年1月28日
 *      Author: 欢悦的骷髅兵
 */
#include"balance.h"
#include"camera.h"
#include"headfile.h"
#define Servo_Delta 150            //舵机左右转动的差值，与舵机型号，拉杆和舵机臂长有关
#define Servo_Center_Mid 600      //舵机直行中值，
#define Servo_Left_Max   (Servo_Center_Mid+Servo_Delta)      //舵机左转极限值
#define Servo_Right_Min  (Servo_Center_Mid-Servo_Delta)      //舵机右转极限值，此值跟舵机放置方式有关，立式

uint8  Start_Flag=0;                    //启动标志

float X_Balance_KP=4000.001,X_Balance_KI=4.0001,X_Balance_KD=60.001;  // 平衡环PID  ***Pitch轴角度环****PID参数 动量轮**********111111111111！！！！！！（要调）
float X_Velocity_KP=90,X_Velocity_KI=90;//***动量轮电机速度环***PI参数*******222222222222222！！！！！！！！！（要调）

float Pitch_Zero=6.9,  Pitch_error=0.00;//设置Pitch轴角度零点*******（要调）
float Zero_error = 0.00;              // Pitch偏差值
int PWM_X,PWM_accel;                  // PWM中间量

sint16 MotorDutyA = 0;                // 飞轮电机驱动占空比数值
sint16 MotorDutyB = 0;                // 后轮电机驱动占空比数值

#define Balance_Kp  32                // 舵机PID参数
#define Balance_Kd   0.03             // 舵机PID参数
#define Balance_Ki   0.0020           // 舵机PID参数**********333333333333！！！！！！（要调）

//#define Motor_Kp     12               // 后轮电机PID参数
//#define Motor_Ki     20               // 后轮电机PID参数***********44444444444444！！！！！（要调）

short  Velocity = 30;               // 速度，定时周期内为60个脉冲，512编码器  //即编码器测得值（待调）
short  PWMMotor, PWMServo;            // 电机舵机PMW变量中值
int  Motor_Bias, Motor_Last_Bias, Motor_Integration; // 电机所用参数
float   Integration;//舵机pid的积分值，停车用
int  Servo_PWM;


unsigned char Flag_Stop = 0, Flag_Show, Flag_Imu;          // 停车，显示，IMU完成标志位
extern int Vat;
extern int Vbat;

                        extern  sint16 TempAngle;
                        extern float gyro[3];// 电单车左右角速度
extern sint16 OFFSET0;      //最远处，赛道中心值综合偏移量
extern sint16 OFFSET1;      //第二格
extern sint16 OFFSET2;      //最近，第三格
extern sint16 TXV;          //梯形的左高度，右高度

                        extern float   Pitch;
extern volatile sint16 ECPULSE1;          //速度全局变量
extern volatile sint16 ECPULSE2;          //速度全局变量
extern volatile sint32 RAllPulse;         //速度全局变量



//主要函数：
void Balance(void)
{
    int  Servo_PWM;                             // 舵机PID

    /* 获取编码器值 */
    ECPULSE1 = gpt12_get(GPT12_T2);
    gpt12_clear(GPT12_T2);// 动量轮反馈  编码器1
    ECPULSE2 = gpt12_get(GPT12_T4);
    gpt12_clear(GPT12_T2);// 后轮反馈     编码器2

   // DMP_Read();//pitch 左负右正
    Seek_Road();  // 获取赛道颜色偏差
    TempAngle = OFFSET2+OFFSET1+OFFSET1;    // 计算赛道颜色偏差值
    Zero_error = Pitch_Zero+TempAngle/800;  // 计算倾斜角度偏差值


    /////// 动量轮控制//////////
    PWM_X = X_balance_Control(Pitch,Pitch_Zero,gyro[0]);// 动量轮电机控制左右倾角***角度环
    PWM_accel = - Velocity_Control(-ECPULSE1);          // 动量轮电机*****速度环******正反馈
    MotorDutyA = -(PWM_X-PWM_accel);        //****************(f 电机pwm = 角度环pid - 电机速度环pi)
    /**/
    if(MotorDutyA>8000) MotorDutyA=8000;        // 动量轮电机限幅
    else if(MotorDutyA<-8000) MotorDutyA=-8000; // 动量轮电机限幅
    else if(MotorDutyA<-0) MotorDutyA -=800;    // 死区
    else if(MotorDutyA>0) MotorDutyA+=800;      // 死区
    if((MotorDutyA<1000)&&(MotorDutyA>-1000))
            MotorDutyA=0;
    if(0<=MotorDutyA) //动量轮电机3   正转//引脚要改******（待调）
     {
         pwm_duty(ATOM0_CH4_P02_4, MotorDutyA);
         pwm_duty(ATOM0_CH5_P02_5, 0);
     }
     else                //电机3   反转
     {
         pwm_duty(ATOM0_CH4_P02_4, 0);
         pwm_duty(ATOM0_CH5_P02_5, -MotorDutyA);
     }


    ///////// 舵机电机控制///////
    Servo_PWM = SBB_Get_BalancePID(Pitch, gyro[0]);               // PID计算单车平衡的PWM数值
    if( Servo_PWM < - Servo_Delta)    Servo_PWM = - Servo_Delta;  // 舵机角度限制
    else if(Servo_PWM > Servo_Delta)  Servo_PWM =   Servo_Delta;  // 舵机角度限制
    PWMServo = Servo_Center_Mid + Servo_PWM;                      // 转换为舵机控制PWM

    MotorDutyB = SBB_Get_MotorPI(ECPULSE2, Velocity)/5;           // 后轮电机增量式PI控制****速度环***

    if((Pitch > 20) || (Pitch < -20))       // 摔倒停车判断
       Flag_Stop = 1;
//    if(Vbat<2500) Flag_Stop = 1;            // 电压低于约11V，停止输出，防止电池过放
//    else Flag_Stop = 0;

    if(Flag_Stop == 1)                       // 停车
    {
        MotorDutyA=0;
        MotorDutyB = 0;                      // 电机关闭
        Integration = 0;                     // 积分参数归零
    }


    //***//
    /*舵机控制*///引脚要改*******（待调）
    pwm_duty(ATOM1_CH1_P33_9,PWMServo);
    /*后轮电机控制*///引脚要改
    if(0<=MotorDutyB) //后轮电机   正转*******（待调）
     {
         pwm_duty(ATOM0_CH4_P02_4, MotorDutyB);
         pwm_duty(ATOM0_CH5_P02_5, 0);
     }
     else                //后轮电机   反转
     {
         pwm_duty(ATOM0_CH4_P02_4, 0);
         pwm_duty(ATOM0_CH5_P02_5, -MotorDutyB);
     }

}



/*X轴平衡PID控制,角度环********111***********/
float X_balance_Control(float Angle,float Angle_Zero,float Gyro)
{
     float PWM,Bias;
     static float error;
     Bias=Angle-Pitch_Zero;  //获取偏差


     error+=Bias;                                                      //偏差累积
     if(error>+30) error=+30;                                          //积分限幅
     if(error<-30) error=-30;                                          //积分限幅


     PWM=X_Balance_KP * Bias + X_Balance_KI * error + X_Balance_KD * Gyro;   //获取最终数值
     return PWM;
}



/*动量轮速度PI控制,速度正反馈环****222******/
float Velocity_Control(int encoder)
{
    static float Encoder,Encoder_Integral;
    float Velocity,Encoder_Least;

    Encoder_Least = encoder;                                  //速度滤波
    Encoder *= 0.7;                                           //一阶低通滤波器
    Encoder += Encoder_Least*0.3;                             //一阶低通滤波器

    Encoder_Integral += Encoder;                              //积分出位移
    if(Encoder_Integral > +2000) Encoder_Integral = +2000;    //积分限幅
    if(Encoder_Integral < -2000) Encoder_Integral = -2000;    //积分限幅

    Velocity = Encoder * X_Velocity_KP + Encoder_Integral * X_Velocity_KI/100;
                                                              //获取最终数值
    if(Start_Flag==0) Encoder_Integral=0,Encoder=0,Velocity=0;//停止时参数清零
      return Velocity;
}




/*舵机打角控制PID*********333*******
【参数值】float Angle,电单车左右倾角
【参数值】float Gyro 电单车左右角速度
【返回值】小车平衡，舵机转向PID*/
int SBB_Get_BalancePID(float Angle,float Gyro)
{
    float  Bias;
    int SBB_BalancePID;


    Bias = Angle - Zero_error;     // 求出平衡的角度中值和此时横滚角的偏差
    Integration += Bias;           // 积分
    if(Integration<-380)      Integration=-380; //限幅
    else if(Integration>380)  Integration= 380; //限幅


    //===计算平衡控制的舵机PWM  PID控制 kp是P系数 ki式I系数 kd是D系数
    SBB_BalancePID = Balance_Kp * Bias + Balance_Ki*Integration + Balance_Kd*Gyro;
    return SBB_BalancePID;
}


//
//#define Motor_Kp     1.05             // 后轮电机PID参数
//#define Motor_Ki     2.1               // 后轮电机PID参数***********44444444444444！！！！！（要调）


#define Motor_Kp   4.2             // 后轮电机PID参数
#define Motor_Ki   0.01               // 后轮电机PID参数***********44444444444444！！！！！（要调）
#define Motor_Kd   0.01


int32 integral;
/*后轮电机控制增量式PI******444*********/
int SBB_Get_MotorPI (uint16 Encoder,uint16 Target)
{
    static int Pwm ;
    Motor_Bias = Target - Encoder;
    integral += Motor_Bias;
    lcd_showint16(0,4, integral);

    if((Motor_Ki * integral) <- 1000)      integral= -1000 / Motor_Ki; //限幅
    else if((Motor_Ki * integral) > 1500)  integral =  1500 / Motor_Ki; //限幅

    Pwm = Motor_Kp * Motor_Bias + Motor_Ki * integral + Motor_Kd * (Motor_Bias - Motor_Last_Bias);
    lcd_showint16(0,5, Pwm);
    //==位置式PID
    Motor_Last_Bias = Motor_Bias;


    if(Pwm > 5000) Pwm = 5000;               // 限幅
    else if(Pwm < 0) Pwm = 0;         // 限幅
    return Pwm;

//    static int Pwm ;
////    Motor_Bias = Encoder - Target;            // 计算偏差
//
//    Motor_Bias = Target - Encoder;
////    lcd_showint16(0,0, Encoder);
////    lcd_showint16(0,1,Motor_Bias);

//    Pwm += Motor_Kp * (Motor_Bias - Motor_Last_Bias) + Motor_Ki * Motor_Bias;
//    // ==增量式PI控制器
//    if(Pwm > 15000) Pwm = 15000;               // 限幅
//    else if(Pwm < -6000) Pwm = -6000;         // 限幅
//    Motor_Last_Bias = Motor_Bias;            // 保存上一次偏差
//    return Pwm/3;                              // 增量输出
//
}





