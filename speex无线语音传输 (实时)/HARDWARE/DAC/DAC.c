#include "DAC.h"
#include "ADC.h"

#define DAC_DHR12RD_Address     0x40007420
/*
 * ��������DAC1_GPIO_Config
 * ����  ��ʹ��DAC1��DMA1��ʱ�ӣ���ʼ��PA.04
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
*/
void DAC1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	  /* ʹ�� GPIOA �� DAC ʱ��*/
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC|RCC_APB1Periph_TIM2, ENABLE);//ע��DAC clock is int APB1
	
	  /* ����PA.04Ϊģ����� */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	//GPIO_Pin_4|,//PA4��PA5��DAC1,DAC2
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);			
	  /* TIME1 configuration */
	  TIM_DeInit(TIM2);
	  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	  /* TIM3 used for timing, the timing period depends on the sample rate */
	  TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;    /* TIM2CLK = 72 MHz */
	  TIM_TimeBaseStructure.TIM_Period = 124;             //��Ӧ8K����
	  TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;
	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	  TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_Update); //ѡ��TRGO����ԴΪ��ʱ������ʱ��
    TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE); //ʹ��ָ����TIM3�ж�,���������
	  TIM_Cmd(TIM2, DISABLE);
		//�ж����ȼ�NVIC����
	  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3�ж�
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
   	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	  NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
	  /* ����DAC1 */
	  DAC_DeInit();
	  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;	// DAC_Trigger_T2_TRGO
	  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None ; 	 //������
	  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;		//ʹ����������򿪾�û�����
	  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	  DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ
	  DAC_DMACmd(DAC_Channel_1, ENABLE);/*ʹ��DACָ����DMAͨ������*/
	  DAC_Cmd(DAC_Channel_1,DISABLE);
}
/*DAC��DMA��ʼ��*/
void DAC1_DMA2_Config(void)
{
	  DMA_InitTypeDef  DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);/*ʹ��DMA2ʱ��*/
	
	  DMA_DeInit(DMA2_Channel3);/*��DMA��ͨ��13�Ĵ�������ΪĬ��ֵ*/
	  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_Address;/*����DMA�������ַ*/
	  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&(TrueDACBuff);/*���ڶ���DMA�ڴ����ַ*/
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;/*����������Ϊ���ݴ����Ŀ�ĵ�*/
	  DMA_InitStructure.DMA_BufferSize = VcBuf_Len;/*����DMA����Ĵ�С����λΪ���ݵ�λ������DMA_PeripheralDataSize��DMA_MemoryDataSize��ֵ*/										
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/*���������ַ���䣬��һֱ��DAC_DHR12RD����Ĵ���*/
  	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;/*�ڴ��ַ�Ĵ�������*/
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;/*�趨���������ݿ�ȣ�16λ*/
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;/*�趨���������ݿ��Ϊ16λ*/	
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;/*������ѭ������ģʽ*/
	  DMA_InitStructure.DMA_Priority = DMA_Priority_High;/*��DMAͨ��ӵ�и����ȼ�*/
	  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;/*���ڴ浽�ڴ洫��*/
	  DMA_Init(DMA2_Channel3, &DMA_InitStructure);

	  DMA_ITConfig(DMA2_Channel3, DMA_IT_TC, ENABLE);/*DMAͨ���ж�ʹ��*/
	  DMA_Cmd(DMA2_Channel3, DISABLE);/*ʹ��DMA2 3ͨ��*/
	  /*enable DMA1 channel 1 NVIC*/
	  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel3_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;/* ��ռ���ȼ�0 */
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;/* �����ȼ�2 */
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
}
/*DAC��ʼ��*/
void DAC1_Init(void)
{
	  DAC1_Config();
    DAC1_DMA2_Config();
}
/*DAC����*/
void DAC1Open(void)
{	
	  TIM_Cmd(TIM2, ENABLE);/*ʹ��DACָ����DMAͨ������*/
	  DAC_Cmd(DAC_Channel_1,ENABLE);
	  DMA_Cmd(DMA2_Channel3, ENABLE);/*ʹ��DMA2 3ͨ��*/
}
/*DAC�ر�*/
void DAC1Close(void)
{
	  TIM_Cmd(TIM2, DISABLE);/*ʹ��DACָ����DMAͨ������*/
	  DAC_Cmd(DAC_Channel_1,DISABLE);
	  DMA_Cmd(DMA2_Channel3, DISABLE);/*ʹ��DMA2 3ͨ��*/
}
/* ��������һ��DMA������DMA���� */
void DACTran(void)
{
		DMA_Cmd(DMA2_Channel3, DISABLE);/*ʹ��DMA2 3ͨ��*/
	  DMA_SetCurrDataCounter(DMA2_Channel3, 160); 
	  DMA_Cmd(DMA2_Channel3, ENABLE);/*ʹ��DMA2 3ͨ��*/
}
