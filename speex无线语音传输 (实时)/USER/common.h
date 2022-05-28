#ifndef _COMMON_H
#define _COMMON_H

#include "stm32f10x.h"
#include "DAC.h"
#include "ADC.h"
#include "Key.h"
#include "NVIC.h"
#include "LED.h"
#include "delay.h"
#include "sys.h"
#include "24l01.h"
#include "spi.h"
#include "string.h"
#include "usart.h"
#include "lcd.h"

u8 key=0,TranFlag=0,ReceFlag=0;
	

u8 encode_out[20] = {0};/* 发射端编码后的保存数组 */
u8 decode_out[20] = {0};/* 编码缓冲数组 */

u16 adcval = 0;

__IO uint16_t ADCBuff[160];/* ADC采集数组 */
__IO uint16_t DACBuff[160];/* DAC解码数组 */
__IO uint16_t TrueDACBuff[160];/* DAC的DMA传输数组 */

extern void Speex_Init(void);
extern void Encode(void);
extern void Decode(void);

#endif
