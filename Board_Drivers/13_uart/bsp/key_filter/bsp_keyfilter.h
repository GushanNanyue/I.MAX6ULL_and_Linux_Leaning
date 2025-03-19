#ifndef __BSP_KEYFILTER_H
#define __BSP_KEYFILTER_H
#include "imx6ul.h"

void keyfilter_init(void);
void filtertimer_init(unsigned int value);
void restart_filtertimer(unsigned int value);
void filtertimer_irqhandler(unsigned int gicciar,void* param);
void gpio1_16_31_irqhandler(unsigned int gicciar, void *param);
#endif
