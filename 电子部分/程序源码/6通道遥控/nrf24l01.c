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
/* SPI数据收发函数    */
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
/* 函数功能：给24L01的寄存器写值（一个字节） */
/* 入口参数：reg   要写的寄存器地址          */
/*           value 给寄存器写的值            */
/* 出口参数：status 状态值                   */
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
/* 函数功能：读24L01的寄存器值 （一个字节）      */
/* 入口参数：reg  要读的寄存器地址               */
/* 出口参数：value 读出寄存器的值                */
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
/* 函数功能：读24L01的寄存器值（多个字节）   */
/* 入口参数：reg   寄存器地址                */
/*           *pBuf 读出寄存器值的存放数组    */
/*           len   数组字节长度              */
/* 出口参数：status 状态值                   */
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
/* 函数功能：给24L01的寄存器写值（多个字节）  */
/* 入口参数：reg  要写的寄存器地址            */
/*           *pBuf 值的存放数组               */
/*           len   数组字节长度               */
/**********************************************/
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u8 status, u8_ctr;
	NRF_CSN = 0;
	status = SPI_RW(reg); //发送寄存器值(位置),并读取状态值
	for (u8_ctr = 0; u8_ctr < len; u8_ctr++)
		SPI_RW(*pBuf++); //写入数据
	NRF_CSN = 1;
	return status; //返回读到的状态值
}

/*********************************************/
/* 函数功能：24L01接收数据                   */
/* 入口参数：rxbuf 接收数据数组              */
/* 返回值： 0   成功收到数据                 */
/*          1   没有收到数据                 */
/*********************************************/
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 state;
	unsigned char revale = 0;

	state = NRF24L01_Read_Reg(STATUS);			//读取状态寄存器的值
	NRF24L01_RW_Reg(WRITE_REG + STATUS, state); //清除TX_DS或MAX_RT中断标志
	if (state & RX_OK)							//接收到数据
	{
		NRF_CE = 0;											   // SPI使能
		NRF24L01_Read_Buf(RD_RX_PLOAD, rxbuf, RX_PLOAD_WIDTH); // read receive payload from RX_FIFO buffer
		NRF24L01_RW_Reg(FLUSH_RX, 0xff);					   //清除RX FIFO寄存器
		revale = 1;
		NRF_CE = 1;
	}

	return revale; //没收到任何数据
}

/**********************************************/
/* 函数功能：设置24L01为发送模式              */
/* 入口参数：txbuf  发送数据数组              */
/* 返回值； 0x10    达到最大重发次数，发送失败*/
/*          0x20    成功发送完成              */
/*          0xff    发送失败                  */
/**********************************************/
u8 NRF24L01_TxPacket(u8 *txbuf)
{
	u8 state;
	unsigned int delay = 0;
	u8 ret = 0;

	NRF_CE = 0; // StandBy I模式
	// NRF24L01_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 装载接收端地址
	NRF24L01_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH); // 装载数据
	// NRF24L01_RW_Reg(WRITE_REG + CONFIG, 0x0e);   		 // IRQ收发完成中断响应，16位CRC，主发送
	NRF_CE = 1;			 //置高CE，激发数据发送
	while (NRF_IRQ == 1) //等待发送完成
	{
		Delay1ms();
		delay++;
		if (delay > 1000)
		{
			ret |= IRQ_TIMEOUT;
			break;
		}
	}
	state = NRF24L01_Read_Reg(STATUS);			//读取状态寄存器的值
	NRF24L01_RW_Reg(WRITE_REG + STATUS, state); //清除TX_DS或MAX_RT中断标志
	if (state & MAX_TX)							//达到最大重发次数
	{
		NRF24L01_RW_Reg(FLUSH_TX, 0xff); //清除TX FIFO寄存器
		ret |= MAX_TX;
	}
	if (state & TX_OK) //发送完成
	{
		ret |= TX_OK;
	}
	return ret; 
}
/********************************************/
/* 函数功能：检测24L01是否存在              */
/* 返回值；  0  存在                        */
/*           1  不存在                      */
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
	NRF24L01_Write_Buf(WRITE_REG + TX_ADDR, (u8 *)TX_ADDRESS, TX_ADR_WIDTH);	//写TX节点地址
	NRF24L01_Write_Buf(WRITE_REG + RX_ADDR_P0, (u8 *)RX_ADDRESS, RX_ADR_WIDTH); //设置Rx节点地址,主要为了使能ACK

	NRF24L01_RW_Reg(WRITE_REG + EN_AA, 0x01);			   //使能通道0的自动应答
	NRF24L01_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);		   //使能通道0的接收地址
	NRF24L01_RW_Reg(WRITE_REG + SETUP_RETR, 0xfa);		   //设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
	NRF24L01_RW_Reg(WRITE_REG + RF_CH, 120);			    
	NRF24L01_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);  //选择通道0的有效数据宽度

	NRF24L01_RW_Reg(WRITE_REG + RF_SETUP, 0x0e); // 7db增益,250kbps
	NRF24L01_RW_Reg(WRITE_REG + CONFIG, 0x0f);	 //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	NRF_CE = 1;									 // CE置高
}

void NRF24L01_TX_Mode(void)
{
	NRF_CE = 0;
	NRF24L01_Write_Buf(WRITE_REG + TX_ADDR, (u8 *)TX_ADDRESS, TX_ADR_WIDTH);	//写TX节点地址
	NRF24L01_Write_Buf(WRITE_REG + RX_ADDR_P0, (u8 *)RX_ADDRESS, RX_ADR_WIDTH); //设置Rx节点地址,主要为了使能ACK

	NRF24L01_RW_Reg(WRITE_REG + EN_AA, 0x01);			   //使能通道0的自动应答
	NRF24L01_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);		   //使能通道0的接收地址
	NRF24L01_RW_Reg(WRITE_REG + SETUP_RETR, 0xfa);		   //设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次，程序等待回应是需要等待5ms就可以了
	NRF24L01_RW_Reg(WRITE_REG + RF_CH, 120);			    
	NRF24L01_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //选择通道0的有效数据宽度

	NRF24L01_RW_Reg(WRITE_REG + RF_SETUP, 0x0e);		   //7db增益,250kbps
	NRF24L01_RW_Reg(WRITE_REG + CONFIG, 0x0e);			   //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	NRF_CE = 1;											   // CE为高,10us后启动发送
}