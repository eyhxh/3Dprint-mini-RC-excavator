

#include "system.h"
#include "nrf24l01.h"
#include "string.h"
#include "stdio.h"

#define LED_G P27
#define LED_R P26

#define LED_ON 0
#define LED_OFF 1

void delay()
{
    int i, j;

    for (i = 0; i < 1000; i++)
        for (j = 0; j < 500; j++)
            ;
}

void motor_test()
{
    P25 = 0;
    P24 = 1;

    P23 = 0;
    P22 = 1;

    P21 = 0;
    P20 = 1;

    P37 = 0;
    P36 = 1;

    P35 = 0;
    P34 = 1;

    P33 = 0;
    P32 = 1;
    sendString("motor foreward\r\n");
    delay();
    P25 = 1;
    P24 = 0;

    P23 = 1;
    P22 = 0;

    P21 = 1;
    P20 = 0;

    P37 = 1;
    P36 = 0;

    P35 = 1;
    P34 = 0;

    P33 = 1;
    P32 = 0;
    sendString("motor backward\r\n");
    delay();
}

void motor0(u8 pwm)
{
    if (pwm > 177)
    {
        P25 = 1;
        P24 = 0;
    }
    else if (pwm < 77)
    {
        P25 = 0;
        P24 = 1;
    }
    else
    {
        P25 = 0;
        P24 = 0;
    }
}
void motor1(u8 pwm)
{
    if (pwm > 177)
    {
        P23 = 1;
        P22 = 0;
    }
    else if (pwm < 77)
    {
        P23 = 0;
        P22 = 1;
    }
    else
    {
        P23 = 0;
        P22 = 0;
    }
}
void motor2(u8 pwm)
{
    if (pwm > 177)
    {
        P21 = 1;
        P20 = 0;
    }
    else if (pwm < 77)
    {
        P21 = 0;
        P20 = 1;
    }
    else
    {
        P21 = 0;
        P20 = 0;
    }
}
void motor3(u8 pwm)
{
    if (pwm > 177)
    {
        P37 = 1;
        P36 = 0;
    }
    else if (pwm < 77)
    {
        P37 = 0;
        P36 = 1;
    }
    else
    {
        P37 = 0;
        P36 = 0;
    }
}
void motor4(u8 pwm)
{
    if (pwm > 177)
    {
        P35 = 1;
        P34 = 0;
    }
    else if (pwm < 77)
    {
        P35 = 0;
        P34 = 1;
    }
    else
    {
        P35 = 0;
        P34 = 0;
    }
}
void motor5(u8 pwm)
{
    if (pwm > 177)
    {
        P33 = 1;
        P32 = 0;
    }
    else if (pwm < 77)
    {
        P33 = 0;
        P32 = 1;
    }
    else
    {
        P33 = 0;
        P32 = 0;
    }
}

#if 0
void motorCtrl(u8 idx, u8 value)
{
    if (idx == 0)
    {
        motor0(value);
    }
    else if (idx == 1)
    {
        motor1(value);
    }
    else if (idx == 2)
    {
        motor2(value);
    }
    else if (idx == 3)
    {
        motor3(value);
    }
    else if (idx == 4)
    {
        motor4(value);
    }
    else if (idx == 5)
    {
        motor5(value);
    }
}
#else

#define MAIN_Fosc 11059200UL //定义主时钟
#define Timer0_Rate 25000    //中断频率

#define Timer0_Reload (65536UL - (MAIN_Fosc / Timer0_Rate)) // Timer 0 重装值

//************** PWM8 变量和常量以及IO口定义 ***************
//********************  8通道8 bit 软PWM    ********************

#define PWM_DUTY_MAX 10 // 0~255    PWM周期, 最大255
#define PWM_ON 1        // 定义占空比的电平, 1 或 0

#define PWM_OFF (!PWM_ON)
#define PWM_ALL_ON (0xff * PWM_ON)

u8 pwm_duty; //周期计数值
u8 pwm[16];  // pwm0~pwm15 为0至15路PWM的宽度值

u8 bdata PWM_temp1, PWM_temp2; //影射一个RAM，可位寻址，输出时同步刷新
sbit P_PWM0 = PWM_temp1 ^ 0;   //  定义影射RAM每位对应的IO
sbit P_PWM1 = PWM_temp1 ^ 1;
sbit P_PWM2 = PWM_temp1 ^ 2;
sbit P_PWM3 = PWM_temp1 ^ 3;
sbit P_PWM4 = PWM_temp1 ^ 4;
sbit P_PWM5 = PWM_temp1 ^ 5;
sbit P_PWM6 = PWM_temp1 ^ 6;
sbit P_PWM7 = PWM_temp1 ^ 7;
sbit P_PWM8 = PWM_temp2 ^ 0;
sbit P_PWM9 = PWM_temp2 ^ 1;
sbit P_PWM10 = PWM_temp2 ^ 2;
sbit P_PWM11 = PWM_temp2 ^ 3;
sbit P_PWM12 = PWM_temp2 ^ 4;
sbit P_PWM13 = PWM_temp2 ^ 5;
sbit P_PWM14 = PWM_temp2 ^ 6;
sbit P_PWM15 = PWM_temp2 ^ 7;

//定义所有io口
#if 0
sbit pwmio0 = P3 ^ 2;
sbit pwmio1 = P3 ^ 3;
sbit pwmio2 = P3 ^ 4;
sbit pwmio3 = P3 ^ 5;
sbit pwmio4 = P3 ^ 6;
sbit pwmio5 = P3 ^ 7;
sbit pwmio6 = P2 ^ 0;
sbit pwmio7 = P2 ^ 1;
sbit pwmio8 = P2 ^ 2;
sbit pwmio9 = P2 ^ 3;
sbit pwmio10 = P2 ^ 4;
sbit pwmio11 = P2 ^ 5;
#endif
sbit pwmio0 = P2 ^ 2;
sbit pwmio1 = P2 ^ 3;

sbit pwmio2 = P3 ^ 6;
sbit pwmio3 = P3 ^ 7;

sbit pwmio4 = P2 ^ 0;
sbit pwmio5 = P2 ^ 1;

sbit pwmio6 = P2 ^ 4;
sbit pwmio7 = P2 ^ 5;

sbit pwmio8 = P3 ^ 4;
sbit pwmio9 = P3 ^ 5;

sbit pwmio10 = P3 ^ 2;
sbit pwmio11 = P3 ^ 3;

void softPWMInit()
{
    AUXR |= (1 << 7);          // Timer0 set as 1T mode
    TMOD &= ~(1 << 2);         // Timer0 set as Timer
    TMOD &= ~0x03;             // Timer0 set as 16 bits Auto Reload
    TH0 = Timer0_Reload / 256; // Timer0 Load
    TL0 = Timer0_Reload % 256;
    ET0 = 1; // Timer0 Interrupt Enable
    PT0 = 1; //高优先级
    TR0 = 1; // Timer0 Run
    EA = 1;  //打开总中断
}

/********************** Timer0 1ms中断函数 ************************/
void timer0(void) interrupt 1
{
    // P1 = PWM_temp1;         //影射RAM输出到实际的PWM端口
    // P2 = PWM_temp2;
    pwmio0 = P_PWM0;
    pwmio1 = P_PWM1;
    pwmio2 = P_PWM2;
    pwmio3 = P_PWM3;
    pwmio4 = P_PWM4;
    pwmio5 = P_PWM5;
    pwmio6 = P_PWM6;
    pwmio7 = P_PWM7;
    pwmio8 = P_PWM8;
    pwmio9 = P_PWM9;
    pwmio10 = P_PWM10;
    pwmio11 = P_PWM11;
    if (++pwm_duty == PWM_DUTY_MAX) // PWM周期结束，重新开始新的周期
    {
        pwm_duty = 0;
        PWM_temp1 = PWM_ALL_ON;
        PWM_temp2 = PWM_ALL_ON;
    }
    ACC = pwm_duty;
    if (ACC == pwm[0])
        P_PWM0 = PWM_OFF; //判断PWM占空比是否结束
    if (ACC == pwm[1])
        P_PWM1 = PWM_OFF;
    if (ACC == pwm[2])
        P_PWM2 = PWM_OFF;
    if (ACC == pwm[3])
        P_PWM3 = PWM_OFF;
    if (ACC == pwm[4])
        P_PWM4 = PWM_OFF;
    if (ACC == pwm[5])
        P_PWM5 = PWM_OFF;
    if (ACC == pwm[6])
        P_PWM6 = PWM_OFF;
    if (ACC == pwm[7])
        P_PWM7 = PWM_OFF;
    if (ACC == pwm[8])
        P_PWM8 = PWM_OFF;
    if (ACC == pwm[9])
        P_PWM9 = PWM_OFF;
    if (ACC == pwm[10])
        P_PWM10 = PWM_OFF;
    if (ACC == pwm[11])
        P_PWM11 = PWM_OFF;
}

// for test idx 0~5  pwmvalue 0-128
void setPWM(u8 idx, u8 pwmValue)
{
    if(pwmValue <= 20) //避开摇杆的中间位置
        pwmValue = 0;
    else
        pwmValue -= 20;

    pwmValue /= 10; 

    if(pwmValue > 10) // 物理pwm范围 0-10
        pwmValue = 10;

    if (idx < 12)
    {
        pwm[idx] = pwmValue;
    }
}


/*
    0~255 input
*/
void motorCtrl(u8 index, u8 pwmvalue, u8 reverse)
{
    int pwmin = pwmvalue;

    int val;
    if (reverse)
        val = 128 - pwmin;
    else
        val = pwmin - 128;

//val范围 -128 ~ 127
    if (val > 0)
    {
        setPWM(index * 2, val);
        setPWM(index * 2 + 1, 0);
    }
    else if (val < 0)
    {
        setPWM(index * 2, 0);
        setPWM(index * 2 + 1, -val);
    }
    else
    {
        setPWM(index * 2, 0);
        setPWM(index * 2 + 1, 0);
    }
}

#endif

idata u8 nrf_rx_buf[32];
#if 0
xdata u8 buf[64] = {0};
void adcSendUart()
{
    memset(buf, 0, 64);
    sprintf(buf, "%bx %bx %bx %bx %bx %bx %bx %bx\r\n",
            adcvalue[0], adcvalue[1], adcvalue[2], adcvalue[3], adcvalue[4], adcvalue[5], adcvalue[6], adcvalue[7]);

    sendString(buf);
}
#endif
void main()
{
    delay_ms(100); // 延时待系统稳定
    softPWMInit();

    //引脚复用初始化
    P1M0 = (1 << 2) + (1 << 3) + (1 << 4) + (1 << 5); // CE CSN SCK MOSI 设为PP输出 MISO IRQ默认
    P1M1 = 0x0;

    P2M0 = (1 << 0) + (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4) + (1 << 5); //电机输出设为pp输出
    P2M1 = 0x0;

    P3M0 = (1 << 2) + (1 << 3) + (1 << 4) + (1 << 5) + (1 << 6) + (1 << 7);
    P3M1 = 0x0;
    // NRF_CE = 1;

    serial_open(); // 打开串口
    sendString("hello init\r\n");
    // 等待检测到NRF24L01，程序才会向下执行
    while (NRF24L01_Check())
    {
        LED_R = LED_ON;
        sendString("check nrf...\r\n");
        delay();
    }
    LED_R = LED_OFF;
    sendString("check nrf ok!\r\n");
    NRF24L01_RX_Mode(); // 配置NRF24L01为接收模式

    while (1)
    {
#if 0
        NRF_CE = 0;
        NRF24L01_RW_Reg(WRITE_REG + CONFIG, 0x0f); // IRQ收发完成中断响应，16位CRC	，主接收
				NRF_CE = 1;
#endif

        if (NRF24L01_RxPacket(nrf_rx_buf)) // || !NRF_IRQ)
        {
            // STA_LED=~STA_LED; // 翻转指示灯
            // for(i=0;i<32;i++)
            // senddata(rece_buf[i]);
            // sendString("rev data\r\n");
            /*
                电机排序  0挖斗   1小臂   2大臂  3旋转  4左 5右
                遥控信号排序  0左y 1左x  2右y 3右x  4副1y   5副2y


            */
            motorCtrl(0, nrf_rx_buf[0], 1);
            motorCtrl(1, nrf_rx_buf[1], 0);
            motorCtrl(2, nrf_rx_buf[2], 1);
            motorCtrl(3, nrf_rx_buf[3], 1);
            motorCtrl(4, nrf_rx_buf[4], 0);
            motorCtrl(5, nrf_rx_buf[5], 0);

            LED_G = LED_ON;
            delay_ms(2);
            LED_G = LED_OFF;
        }
        else
        {
            delay_ms(10);
        }
    }
}
