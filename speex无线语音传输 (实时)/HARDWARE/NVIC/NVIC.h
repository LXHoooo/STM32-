#ifndef _NVIC_H
#define _NVIC_H

#include "stm32f10x.h"
#include "LED.h"

extern u8 TranFlag,ReceFlag;/* ¼�ű�־λ */

extern u16 adcval;/* ADC����ֵ */
extern u32 dacval;/* DAC����ֵ */

extern __IO uint16_t ADCBuff[];/* ADC������������ */
extern __IO uint16_t DACBuff[];/* DAC���뻺������ */
extern __IO uint16_t TrueDACBuff[];/* DACʵ�ʲ����������� */
extern u8 encode_out[];/* ѹ���������� */
extern u8 decode_out[];/* ѹ���������� */

extern void Encode(void);/* ���뺯�� */
extern void Decode(void);/* ���뺯�� */
extern void ADC1Close(void);/* ֹͣADC���� */
extern void DAC1Close(void);/* ֹͣADC���� */
extern void DACTran(void);
extern void ADCTran(void);
#endif

