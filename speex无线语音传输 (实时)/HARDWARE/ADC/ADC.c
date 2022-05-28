#include "ADC.h"
#include "SEGGER_RTT.h"
#include "delay.h"

#define ADC1_DR_Address    ((uint32_t)0x40012400+0x4c)


void ADC1_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;	//GPIO��ʼ���ṹ
		ADC_InitTypeDef ADC_InitStructure;		//ADC1��ʼ���ṹ
	  NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		//��GPIO��ADC1��ʱ��
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA , ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		

		//���ö�ӦGPIO�ڣ�����ʹ����PA0��ADC1   ͨ��0
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	//
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;/*ģ������*/
		GPIO_Init(GPIOA, &GPIO_InitStructure);				// PA0,����ʱ������������
		
		/* TIME1 configuration */
		TIM_DeInit(TIM3);
		TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
		/* TIM3 used for timing, the timing period depends on the sample rate */
		TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;/* TIM3CLK = 72 MHz */
		TIM_TimeBaseStructure.TIM_Period = 124;/*��������������ʱ�䣬�Ӷ��õ����Ƶ�ʣ�����ADC�ɼ��������8K����*/
		TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;/*���ϼ���*/
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
		TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Update);//ѡ��TRGO����ԴΪ��ʱ������ʱ��
		TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //ʹ��ָ����TIM3�ж�,���������
		TIM_Cmd(TIM3, DISABLE);/*��������*/
		//�ж����ȼ�NVIC����
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0������ԽС�����ȼ�Խ��
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�1����ռ���ȼ����������ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
		NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
		//����ADC1�Ĺ���ģʽ
		ADC_DeInit(ADC1);		//��ʼ��
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;			/*����ADCģʽ*/
		ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 	 				/*����ɨ��ģʽ��ɨ��ģʽ���ڶ�ͨ���ɼ�*/
		ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;			/*����ת��ģʽ��һ��ת�����EOC��λ��ADCֹͣ�����������߶�ȡADC_DR���*/
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;/*ѡ��ʱ��3��TRGO��Ϊת���ⲿ����*/
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	/*�ɼ������Ҷ���*/
		ADC_InitStructure.ADC_NbrOfChannel = 1;	 								/*Ҫת����ͨ����Ŀ1*/
		ADC_Init(ADC1, &ADC_InitStructure);
		
		/*����ADCʱ�ӣ�ΪPCLK2��6��Ƶ����12MHz*/
		RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
		//����ADC1��ת��˳��Ͳ�������
		ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);	
		ADC_DMACmd(ADC1,ENABLE);/*��������*/
		ADC_Cmd(ADC1, ENABLE);/*��ʼ����ʱ��һ��Ҫʹ��ADC1������ADC��ʼ�������ȥ*/
		
		ADC_ResetCalibration(ADC1);/*��λУ׼�Ĵ���*/		
		while(ADC_GetResetCalibrationStatus(ADC1));/*�ȴ�У׼�Ĵ�����λ��� */
		ADC_StartCalibration(ADC1);/*ADCУ׼ */
		while(ADC_GetCalibrationStatus(ADC1));/*�ȴ�У׼���*/
		ADC_ExternalTrigConvCmd(ADC1, ENABLE);/*����ADC�ⲿ����ģʽ*/
}
void ADC1_DMA1_Config()
{
		DMA_InitTypeDef DMA_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		
		/* DMA channel1 ���� */
		DMA_DeInit(DMA1_Channel1);
		DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;/*ADC1�ĵ�ַ*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&(adcval);/*���ݴ洢��ַ*/
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_BufferSize = 1;/*��������ݸ���*/
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/*�����ַ�̶�,��ADC->DR�Ĵ�����ַ�̶�*/
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;/*���ݴ洢��ַ�̶�*/
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;/*���֣�2���ֽ����ݴ���*/
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;/*���֣�2���ֽ����ݴ���*/
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;/*���δ���*/
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;/* ָ��ͨ�������ȼ� */
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;/*���ڴ浽�ڴ�ģʽ*/
		DMA_Init(DMA1_Channel1, &DMA_InitStructure);
		/* Enable DMA channel1 */
		DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
		DMA_Cmd(DMA1_Channel1, DISABLE);	//��������
		/*enable DMA1 channel 1 NVIC*/
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;/* ��ռ���ȼ�0 */
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;/* �����ȼ�2 */
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
}
/*ADC1��ʼ��*/
void ADC1_Init(void)
{
		ADC1_Config();	//����ADC
		ADC1_DMA1_Config();	//����DMA
}
/*ADC1�ر�*/
void ADC1Close(void)
{
	  TIM_Cmd(TIM3, DISABLE);/*��������*/
    ADC_Cmd(ADC1, DISABLE);/*ʹ��ADC1*/
  	DMA_Cmd(DMA1_Channel1, DISABLE);	//��������	
}
/*ADC1�ر�*/
void ADC1Open(void)
{
	  TIM_Cmd(TIM3, ENABLE);/*��������*/
	  ADC_Cmd(ADC1, ENABLE);/*ʹ��ADC1*/
    DMA_Cmd(DMA1_Channel1, ENABLE);	//��������
}
/* ��������һ��DMA������DMA���� */
void ADCTran(void)
{
		DMA_Cmd(DMA1_Channel1, DISABLE);/*ʹ��DMA1 1ͨ��*/
	  DMA_SetCurrDataCounter(DMA1_Channel1, 160); 
	  DMA_Cmd(DMA1_Channel1, ENABLE);/*ʹ��DMA1 1ͨ��*/
}
