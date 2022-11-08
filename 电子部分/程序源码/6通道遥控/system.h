/******************************************/
//
// 该程序工作的主频是33.1776MHz
//
/******************************************/
#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stc15wxx.h>
	
#define MAIN_Fosc       11059200UL  //定义主时钟
	
typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;
typedef     signed char s8;



void delay_ms(unsigned char ms);
	
#endif