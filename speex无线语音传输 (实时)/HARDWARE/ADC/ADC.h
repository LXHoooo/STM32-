#ifndef _ADC_H
#define _ADC_H

#include "stm32f10x.h"

#define	VcBuf_Len	160/*语音缓存区长度 单位点数 每个采样点16位*/

extern __IO uint16_t ADCBuff[160];

extern u16 adcval;/* ADC采样值 */

void ADC1_Config(void);
void ADC1_Init(void);
void ADC1Open(void);
void ADC1Close(void);

#endif



