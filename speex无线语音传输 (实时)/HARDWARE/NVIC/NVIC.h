#ifndef _NVIC_H
#define _NVIC_H

#include "stm32f10x.h"
#include "LED.h"

extern u8 TranFlag,ReceFlag;/* 录放标志位 */

extern u16 adcval;/* ADC采样值 */
extern u32 dacval;/* DAC播放值 */

extern __IO uint16_t ADCBuff[];/* ADC采样缓存数组 */
extern __IO uint16_t DACBuff[];/* DAC解码缓存数组 */
extern __IO uint16_t TrueDACBuff[];/* DAC实际采样缓存数组 */
extern u8 encode_out[];/* 压缩缓存数组 */
extern u8 decode_out[];/* 压缩缓存数组 */

extern void Encode(void);/* 编码函数 */
extern void Decode(void);/* 解码函数 */
extern void ADC1Close(void);/* 停止ADC采样 */
extern void DAC1Close(void);/* 停止ADC采样 */
extern void DACTran(void);
extern void ADCTran(void);
#endif

