#include "nrf24l01.h"

const u8 TX_ADDRESS[TX_ADR_WIDTH] = {0x34, 0x43, 0x10, 0x10, 0x01};
const u8 RX_ADDRESS[RX_ADR_WIDTH] = {0x34, 0x43, 0x10, 0x10, 0x01};

static void Delay1ms() //@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j)
			;
	} while (--i);
}

/**********************/
/* SPI�����շ�����    */
/**********************/
u8 SPI_RW(u8 tr_data)
{
	u16 bit_ctr;
	for (bit_ctr = 0; bit_ctr < 8; bit_ctr++) // output 8-bit
	{
		NRF_MOSI = (tr_data & 0x80); // output 'uchar', MSB to MOSI
		//Delay1ms();
		tr_data = (tr_data << 1); // shift next bit into MSB..
		NRF_SCK = 1;			  // Set SCK high..
		//Delay1ms();
		tr_data |= NRF_MISO; // capture current MISO bit
		NRF_SCK = 0;		 // ..then set SCK low again
	}
	return (tr_data); // return read uchar
}

/*********************************************/
/* �������ܣ���24L01�ļĴ���дֵ��һ���ֽڣ� */
/* ��ڲ�����reg   Ҫд�ļĴ�����ַ          */
/*           value ���Ĵ���д��ֵ            */
/* ���ڲ�����status ״ֵ̬                   */
/*********************************************/
u8 NRF24L01_RW_Reg(u8 reg, u8 value)
{
	u16 status;

	NRF_CSN = 0; // CSN low, init SPI transaction
	//Delay1ms();
	status = SPI_RW(reg); // select register
	SPI_RW(value);		  // ..and write value to it..
	//Delay1ms();
	NRF_CSN = 1; // CSN high again

	return (status); // return nRF24L01 status uchar
}
/*************************************************/
/* �������ܣ���24L01�ļĴ���ֵ ��һ���ֽڣ�      */
/* ��ڲ�����reg  Ҫ���ļĴ�����ַ               */
/* ���ڲ�����value �����Ĵ�����ֵ                */
/*************************************************/
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;

	NRF_CSN = 0; // CSN low, initialize SPI communication...
	//Delay1ms();
	SPI_RW(reg);		 // Select register to read from..
	reg_val = SPI_RW(0); // ..then read registervalue
	//Delay1ms();
	NRF_CSN = 1; // CSN high, terminate SPI communication

	return (reg_val); // return register value
}
/*********************************************/
/* �������ܣ���24L01�ļĴ���ֵ������ֽڣ�   */
/* ��ڲ�����reg   �Ĵ�����ַ                */
/*           *pBuf �����Ĵ���ֵ�Ĵ������    */
/*           len   �����ֽڳ���              */
/* ���ڲ�����status ״ֵ̬                   */
/*********************************************/
u8 NRF24L01_Read_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u16 status, uchar_ctr;

	NRF_CSN = 0;		  // Set CSN low, init SPI tranaction
	status = SPI_RW(reg); // Select register to write to and read status uchar

	for (uchar_ctr = 0; uchar_ctr < len; uchar_ctr++)
		pBuf[uchar_ctr] = SPI_RW(0); //

	NRF_CSN = 1;

	return (status); // return nRF24L01 status uchar
}
/**********************************************/
/* �������ܣ���24L01�ļĴ���дֵ������ֽڣ�  */
/* ��ڲ�����reg  Ҫд�ļĴ�����ַ            */
/*           *pBuf ֵ�Ĵ������               */
/*           len   �����ֽڳ���               */
/**********************************************/
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u8 status, u8_ctr;
	NRF_CSN = 0;
	status = SPI_RW(reg); //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	for (u8_ctr = 0; u8_ctr < len; u8_ctr++)
		SPI_RW(*pBuf++); //д������
	NRF_CSN = 1;
	return status; //���ض�����״ֵ̬
}

/*********************************************/
/* �������ܣ�24L01��������                   */
/* ��ڲ�����rxbuf ������������              */
/* ����ֵ�� 0   �ɹ��յ�����                 */
/*          1   û���յ�����                 */
/*********************************************/
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 state;
	unsigned char revale = 0;

	state = NRF24L01_Read_Reg(STATUS);			//��ȡ״̬�Ĵ�����ֵ
	NRF24L01_RW_Reg(WRITE_REG + STATUS, state); //���TX_DS��MAX_RT�жϱ�־
	if (state & RX_OK)							//���յ�����
	{
		NRF_CE = 0;											   // SPIʹ��
		NRF24L01_Read_Buf(RD_RX_PLOAD, rxbuf, RX_PLOAD_WIDTH); // read receive payload from RX_FIFO buffer
		NRF24L01_RW_Reg(FLUSH_RX, 0xff);					   //���RX FIFO�Ĵ���
		revale = 1;
		NRF_CE = 1;
	}

	return revale; //û�յ��κ�����
}

/**********************************************/
/* �������ܣ�����24L01Ϊ����ģʽ              */
/* ��ڲ�����txbuf  ������������              */
/* ����ֵ�� 0x10    �ﵽ����ط�����������ʧ��*/
/*          0x20    �ɹ��������              */
/*          0xff    ����ʧ��                  */
/**********************************************/
u8 NRF24L01_TxPacket(u8 *txbuf)
{
	u8 state;
	unsigned int delay = 0;
	u8 ret = 0;

	NRF_CE = 0; // StandBy Iģʽ
	// NRF24L01_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // װ�ؽ��ն˵�ַ
	NRF24L01_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH); // װ������
	// NRF24L01_RW_Reg(WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����ж���Ӧ��16λCRC��������
	NRF_CE = 1;			 //�ø�CE���������ݷ���
	while (NRF_IRQ == 1) //�ȴ��������
	{
		Delay1ms();
		delay++;
		if (delay > 1000)
		{
			ret |= IRQ_TIMEOUT;
			break;
		}
	}
	state = NRF24L01_Read_Reg(STATUS);			//��ȡ״̬�Ĵ�����ֵ
	NRF24L01_RW_Reg(WRITE_REG + STATUS, state); //���TX_DS��MAX_RT�жϱ�־
	if (state & MAX_TX)							//�ﵽ����ط�����
	{
		NRF24L01_RW_Reg(FLUSH_TX, 0xff); //���TX FIFO�Ĵ���
		ret |= MAX_TX;
	}
	if (state & TX_OK) //�������
	{
		ret |= TX_OK;
	}
	return ret; 
}
/********************************************/
/* �������ܣ����24L01�Ƿ����              */
/* ����ֵ��  0  ����                        */
/*           1  ������                      */
/********************************************/
u8 NRF24L01_Check(void)
{
	u8 check_in_buf[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
	u8 check_out_buf[20] = {0x00};

	NRF_CE = 0;

	NRF24L01_Write_Buf(WRITE_REG + TX_ADDR, check_in_buf, 5);
	NRF24L01_Read_Buf(READ_REG + TX_ADDR, check_out_buf, 5);
	NRF_CE = 1;
	if ((check_out_buf[0] == 0x11) &&
		(check_out_buf[1] == 0x22) &&
		(check_out_buf[2] == 0x33) &&
		(check_out_buf[3] == 0x44) &&
		(check_out_buf[4] == 0x55))
		return 0;

	return 1;
}

void NRF24L01_RX_Mode(void)
{
	NRF_CE = 0;																	// chip enable
	NRF24L01_Write_Buf(WRITE_REG + TX_ADDR, (u8 *)TX_ADDRESS, TX_ADR_WIDTH);	//дTX�ڵ��ַ
	NRF24L01_Write_Buf(WRITE_REG + RX_ADDR_P0, (u8 *)RX_ADDRESS, RX_ADR_WIDTH); //����Rx�ڵ��ַ,��ҪΪ��ʹ��ACK

	NRF24L01_RW_Reg(WRITE_REG + EN_AA, 0x01);			   //ʹ��ͨ��0���Զ�Ӧ��
	NRF24L01_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);		   //ʹ��ͨ��0�Ľ��յ�ַ
	NRF24L01_RW_Reg(WRITE_REG + SETUP_RETR, 0xfa);		   //�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
	NRF24L01_RW_Reg(WRITE_REG + RF_CH, 120);			    
	NRF24L01_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);  //ѡ��ͨ��0����Ч���ݿ��

	NRF24L01_RW_Reg(WRITE_REG + RF_SETUP, 0x0e); // 7db����,250kbps
	NRF24L01_RW_Reg(WRITE_REG + CONFIG, 0x0f);	 //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	NRF_CE = 1;									 // CE�ø�
}

void NRF24L01_TX_Mode(void)
{
	NRF_CE = 0;
	NRF24L01_Write_Buf(WRITE_REG + TX_ADDR, (u8 *)TX_ADDRESS, TX_ADR_WIDTH);	//дTX�ڵ��ַ
	NRF24L01_Write_Buf(WRITE_REG + RX_ADDR_P0, (u8 *)RX_ADDRESS, RX_ADR_WIDTH); //����Rx�ڵ��ַ,��ҪΪ��ʹ��ACK

	NRF24L01_RW_Reg(WRITE_REG + EN_AA, 0x01);			   //ʹ��ͨ��0���Զ�Ӧ��
	NRF24L01_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);		   //ʹ��ͨ��0�Ľ��յ�ַ
	NRF24L01_RW_Reg(WRITE_REG + SETUP_RETR, 0xfa);		   //�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10�Σ�����ȴ���Ӧ����Ҫ�ȴ�5ms�Ϳ�����
	NRF24L01_RW_Reg(WRITE_REG + RF_CH, 120);			    
	NRF24L01_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //ѡ��ͨ��0����Ч���ݿ��

	NRF24L01_RW_Reg(WRITE_REG + RF_SETUP, 0x0e);		   //7db����,250kbps
	NRF24L01_RW_Reg(WRITE_REG + CONFIG, 0x0e);			   //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	NRF_CE = 1;											   // CEΪ��,10us����������
}