#ifndef _DAC_H
#define _DAC_H

#include "stm32f10x.h"

extern  __IO uint16_t TrueDACBuff[160];

void DAC1_Config(void);
void DAC1_Init(void);
void DAC1Open(void);
void DAC1Close(void);

#endif


