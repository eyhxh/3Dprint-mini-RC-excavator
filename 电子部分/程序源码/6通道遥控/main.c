

#include "system.h"
#include "nrf24l01.h"
#include "string.h"
#include "stdio.h"



#define LED_G P26
#define LED_R P27

#define LED_ON 0
#define LED_OFF 1

void delay()
{
    int i, j;

    for (i = 0; i < 1000; i++)
        for (j = 0; j < 500; j++)
            ;
}

/*----------------------------
ADC相关数据结构
----------------------------*/
#define ADC_POWER 0x80   // ADC电源控制位
#define ADC_FLAG 0x10    // ADC完成标志
#define ADC_START 0x08   // ADC起始控制位
#define ADC_SPEEDLL 0x00 // 540个时钟
#define ADC_SPEEDL 0x20  // 360个时钟
#define ADC_SPEEDH 0x40  // 180个时钟
#define ADC_SPEEDHH 0x60 // 90个时钟

u8 ch = 0; // ADC通道号
u8 adcvalue[8] = {0};

/*----------------------------
ADC中断服务程序
----------------------------*/
void adc_isr() interrupt 5
{
    ADC_CONTR &= !ADC_FLAG; //清除ADC中断标志

    // SendData(ch);                   //显示通道号
    // SendData(ADC_RES);              //读取高8位结果并发送到串口
    adcvalue[ch] = ADC_RES;
    //    SendData(ADC_LOW2);           //显示低2位结果

    if (++ch > 7)
        ch = 0; //切换到下一个通道
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch;
}

/*----------------------------
初始化ADC
----------------------------*/
void InitADC()
{
#if 0
    // io复用设置
    P0M0 = 0x00;
    P0M1 = 0x00;
    P1M0 = 0x00;
    P1M1 = 0x00;
    P2M0 = 0x00;
    P2M1 = 0x00;
    P3M0 = 0x00;
    P3M1 = 0x00;
    P4M0 = 0x00;
    P4M1 = 0x00;
    P5M0 = 0x00;
    P5M1 = 0x00;
    P6M0 = 0x00;
    P6M1 = 0x00;
    P7M0 = 0x00;
    P7M1 = 0x00;
#endif

    P1ASF = 0xff; //设置P1口为AD口
    ADC_RES = 0;  //清除结果寄存器
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch;
    delay_ms(10); // ADC上电并延时
    IE = 0xa0;    //使能ADC中断
}

#if 1
xdata u8 buf[32] = {0};
void adcSendUart()
{
    memset(buf, 0, 64);
    sprintf(buf, "%bx %bx %bx %bx %bx %bx %bx %bx\r\n",
            adcvalue[0], adcvalue[1], adcvalue[2], adcvalue[3], adcvalue[4], adcvalue[5], adcvalue[6], adcvalue[7]);

    sendString(buf);
}
#endif
idata u8 TxBuf[8] = {0x00};
u8 ret;
void main()
{
    delay_ms(100); // 延时待系统稳定

    P2M0 = 0x20; // NRF CE 设为推挽
    P2M1 = 0x0;

    LED_G = LED_OFF;
    LED_R = LED_OFF;
    serial_open(); // 打开串口
    sendString("hello init\r\n");

    InitADC(); //初始化ADC

#if 1
    // 等待检测到NRF24L01，程序才会向下执行
    while (NRF24L01_Check())
    {
        LED_R = LED_ON;
        sendString("check nrf...\r\n");
        delay();
    }
    LED_R = LED_OFF;
    sendString("check nrf ok!\r\n");
    NRF24L01_TX_Mode(); // 配置NRF24L01为接收模式
#endif
    while (1)
    {
#if 0
        LED_G = 0;
        LED_R = 0;
        delay_ms(50);
        LED_G = 1;
        LED_R = 1;
        delay_ms(50);
#else        
        //adcSendUart();

        TxBuf[0] = adcvalue[3]; //左y 左x  右y 右x 副1y 副2y
        TxBuf[1] = adcvalue[1];
        TxBuf[2] = adcvalue[6];
        TxBuf[3] = adcvalue[2];
        TxBuf[4] = adcvalue[4];
        TxBuf[5] = adcvalue[5];

        sendString("send nrf\r\n");
        ret = NRF24L01_TxPacket(TxBuf);
        if (ret & TX_OK)
        {
            LED_G = LED_ON;
            delay_ms(10);
            LED_G = LED_OFF;
            sendString("send nrf done\r\n");
        }
        if (ret & IRQ_TIMEOUT)
        {
            LED_R = LED_ON;
            delay_ms(10);
            LED_R = LED_OFF;
            sendString("send nrf irq timeout error\r\n");
        }
        if (ret & MAX_TX)
        {
            LED_R = LED_ON;
            delay_ms(10);
            LED_R = LED_OFF;
            sendString("send nrf max tx error\r\n");
        }

        delay_ms(50);
#endif
    }
}
