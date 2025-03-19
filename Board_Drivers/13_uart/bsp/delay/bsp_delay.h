#ifndef __BSP_DELAY_H
#define __BSP_DELAY_H

#include "imx6ul.h"


/* 函数声明 */
void delay(volatile unsigned int n);
void delay_init(void);
void delay_short(volatile unsigned int n);
void gpt1_irqhandler(unsigned int gicciar,void* param);
void delay_us(unsigned int usdelay);
void delay_ms(unsigned int msdelay);

#endif

