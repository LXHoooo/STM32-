#include "NVIC.h"
#include "24l01.h"

/*DMA1通道中断*/
void DMA1_Channel1_IRQHandler(void) 
{
	static u16 SampleNum = 0;
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)/* 传输完成中断 */
	{
		DMA_ClearITPendingBit(DMA1_IT_TC1);/* 清除DMA中断标志位 */
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
/*DMA2通道中断*/
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
			TrueDACBuff[i] = ((DACBuff[i]>>4)+0x300);/* 对解码出来的数据进行处理 */
		}
		DACTran();/* DMA设置的是Normal模式，所以传输完一次需要重新配配置一下传输的个数来重新开启DMA */
	}
}
