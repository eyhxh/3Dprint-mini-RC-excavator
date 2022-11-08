#include "usart.h"
#include "string.h"
\
/*******************串口配置函数**********************/
void serial_open(void)
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xE0;		//设定定时初值
	T2H = 0xFE;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
}

/******************串口发送数据函数********************/
void senddata(u8 data_buf)
{
	SBUF = data_buf;
	while(!TI);
	TI = 0;
}

void sendString(u8 *string)
{
	while(*string)
	{
		senddata(*string);
		string++;
	}
}


u8 rece_buf[32];

/************串口接收数据函数************************/
bit WaitComm()
{	 
	unsigned int i=0;
	unsigned char j=0;
		
	if(RI)
  {
		rece_buf[++j]=SBUF;	 
		RI=0;
		while(i<1500) 
		{
		  if(RI)
			{
			  rece_buf[++j]=SBUF;
				RI = 0;
				i=0;	
			}
			i++;
		}
		rece_buf[0] = j;		
		return 0;		 
	}
	else
	{
		return 1;		  
	}
}

