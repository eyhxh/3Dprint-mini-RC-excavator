#include "usart.h"
#include "string.h"
\
/*******************�������ú���**********************/
void serial_open(void)
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR |= 0x04;		//��ʱ��2ʱ��ΪFosc,��1T
	T2L = 0xE0;		//�趨��ʱ��ֵ
	T2H = 0xFE;		//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
}

/******************���ڷ������ݺ���********************/
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

/************���ڽ������ݺ���************************/
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

