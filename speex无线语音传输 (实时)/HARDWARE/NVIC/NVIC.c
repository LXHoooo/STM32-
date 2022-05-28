#include "NVIC.h"
#include "24l01.h"

/*DMA1ͨ���ж�*/
void DMA1_Channel1_IRQHandler(void) 
{
	static u16 SampleNum = 0;
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)/* ��������ж� */
	{
		DMA_ClearITPendingBit(DMA1_IT_TC1);/* ���DMA�жϱ�־λ */
		ADCBuff[SampleNum] = (adcval & 0x0fff);
		SampleNum++;
		if(SampleNum==160)
		{
			SampleNum = 0;
			Encode();
			NRF24L01_TxPacket(encode_out);
		}
	}	
}
/*DMA2ͨ���ж�*/
void DMA2_Channel3_IRQHandler(void)
{
	u8 i = 0;
  if(DMA_GetITStatus(DMA2_IT_TC3) != RESET)
	{
		DMA_ClearITPendingBit(DMA2_IT_TC3);
    if(NRF24L01_RxPacket(decode_out)==0)
		{
		  Decode();
		}
		for(i=0;i<160;i++)
		{
			TrueDACBuff[i] = ((DACBuff[i]>>4)+0x300);/* �Խ�����������ݽ��д��� */
		}
		DACTran();/* DMA���õ���Normalģʽ�����Դ�����һ����Ҫ����������һ�´���ĸ��������¿���DMA */
	}
}
