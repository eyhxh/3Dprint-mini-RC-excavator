#ifndef USART_H_
#define USART_H_

#include "system.h"
#include "intrins.h"

extern u8 rece_buf[32];

void serial_open(void);
void senddata(u8 data_buf);
void sendString(u8 *string);

#endif