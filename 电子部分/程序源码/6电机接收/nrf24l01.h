#ifndef NRF24L01_H_
#define NRF24L01_H_

#include "usart.h"
#include "system.h"

sbit NRF_CE = P1^2;
sbit NRF_CSN = P1^3;
sbit NRF_MISO = P1^6;
sbit NRF_MOSI = P1^5;
sbit NRF_SCK = P1^4;
sbit NRF_IRQ = P1^7;

/**********  NRF24L01�Ĵ�����������  ***********/
#define READ_REG        0x00  //�����üĴ���,��5λΪ�Ĵ�����ַ
#define WRITE_REG       0x20  //д���üĴ���,��5λΪ�Ĵ�����ַ
#define RD_RX_PLOAD     0x61  //��RX��Ч����,1~32�ֽ�
#define WR_TX_PLOAD     0xA0  //дTX��Ч����,1~32�ֽ�
#define FLUSH_TX        0xE1  //���TX FIFO�Ĵ���.����ģʽ����
#define FLUSH_RX        0xE2  //���RX FIFO�Ĵ���.����ģʽ����
#define REUSE_TX_PL     0xE3  //����ʹ����һ������,CEΪ��,���ݰ������Ϸ���.
#define NOP             0xFF  //�ղ���,����������״̬�Ĵ���	 
/**********  NRF24L01�Ĵ�����ַ   *************/
#define CONFIG          0x00  //���üĴ�����ַ                             
#define EN_AA           0x01  //ʹ���Զ�Ӧ���� 
#define EN_RXADDR       0x02  //���յ�ַ����
#define SETUP_AW        0x03  //���õ�ַ���(��������ͨ��)
#define SETUP_RETR      0x04  //�����Զ��ط�
#define RF_CH           0x05  //RFͨ��
#define RF_SETUP        0x06  //RF�Ĵ���
#define STATUS          0x07  //״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ��Ĵ���
#define CD              0x09  // �ز����Ĵ���
#define RX_ADDR_P0      0x0A  // ����ͨ��0���յ�ַ
#define RX_ADDR_P1      0x0B  // ����ͨ��1���յ�ַ
#define RX_ADDR_P2      0x0C  // ����ͨ��2���յ�ַ
#define RX_ADDR_P3      0x0D  // ����ͨ��3���յ�ַ
#define RX_ADDR_P4      0x0E  // ����ͨ��4���յ�ַ
#define RX_ADDR_P5      0x0F  // ����ͨ��5���յ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ��������ͨ��0��Ч���ݿ��(1~32�ֽ�) 
#define RX_PW_P1        0x12  // ��������ͨ��1��Ч���ݿ��(1~32�ֽ�) 
#define RX_PW_P2        0x13  // ��������ͨ��2��Ч���ݿ��(1~32�ֽ�) 
#define RX_PW_P3        0x14  // ��������ͨ��3��Ч���ݿ��(1~32�ֽ�) 
#define RX_PW_P4        0x15  // ��������ͨ��4��Ч���ݿ��(1~32�ֽ�)
#define RX_PW_P5        0x16  // ��������ͨ��5��Ч���ݿ��(1~32�ֽ�)
#define FIFO_STATUS     0x17  // FIFO״̬�Ĵ���
/*����������������������������������������������������������������������������������������������������������������������������������������*/
		
/******   STATUS�Ĵ���bitλ����      *******/
#define MAX_TX  	0x10  //�ﵽ����ʹ����ж�
#define TX_OK   	0x20  //TX��������ж�
#define RX_OK   	0x40  //���յ������ж�
#define IRQ_TIMEOUT   	0x80  //IRQ��ʱ

/*����������������������������������������������������������������������������������������������������*/

/*********     24L01���ͽ������ݿ�ȶ���	  ***********/
#define TX_ADR_WIDTH    5   //5�ֽڵ�ַ���
#define RX_ADR_WIDTH    5   //5�ֽڵ�ַ���
#define TX_PLOAD_WIDTH  8  //32�ֽ���Ч���ݿ��
#define RX_PLOAD_WIDTH  8  //32�ֽ���Ч���ݿ��

u8 NRF24L01_Check(void);
void NRF24L01_RT_Init(void);
u8 NRF24L01_TxPacket(u8 *txbuf);
u8 NRF24L01_RxPacket(u8 *rxbuf);
u8 NRF24L01_RW_Reg(u8 reg,u8 value);
void NRF24L01_TX_Mode(void);
void NRF24L01_RX_Mode(void);

#endif