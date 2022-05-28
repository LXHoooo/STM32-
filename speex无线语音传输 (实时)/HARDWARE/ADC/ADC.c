#include "ADC.h"
#include "SEGGER_RTT.h"
#include "delay.h"

#define ADC1_DR_Address    ((uint32_t)0x40012400+0x4c)


void ADC1_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;	//GPIO初始化结构
		ADC_InitTypeDef ADC_InitStructure;		//ADC1初始化结构
	  NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		//打开GPIO和ADC1的时钟
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA , ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		

		//配置对应GPIO口，这里使用了PA0即ADC1   通道0
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	//
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;/*模拟输入*/
		GPIO_Init(GPIOA, &GPIO_InitStructure);				// PA0,输入时不用设置速率
		
		/* TIME1 configuration */
		TIM_DeInit(TIM3);
		TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
		/* TIM3 used for timing, the timing period depends on the sample rate */
		TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;/* TIM3CLK = 72 MHz */
		TIM_TimeBaseStructure.TIM_Period = 124;/*根据这个计算溢出时间，从而得到溢出频率，触发ADC采集，这就是8K采样*/
		TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;/*向上计数*/
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
		TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Update);//选择TRGO触发源为计时器更新时间
		TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //使能指定的TIM3中断,允许更新中
		TIM_Cmd(TIM3, DISABLE);/*按键开启*/
		//中断优先级NVIC设置
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //抢占优先级0，数字越小，优先级越高
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //子优先级1，抢占优先级高于子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
		NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
		//配置ADC1的工作模式
		ADC_DeInit(ADC1);		//初始化
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;			/*独立ADC模式*/
		ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 	 				/*开启扫描模式，扫描模式用于多通道采集*/
		ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;			/*单次转换模式，一次转换完成EOC置位，ADC停止，软件清楚或者读取ADC_DR清除*/
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;/*选择定时器3的TRGO作为转换外部触发*/
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	/*采集数据右对齐*/
		ADC_InitStructure.ADC_NbrOfChannel = 1;	 								/*要转换的通道数目1*/
		ADC_Init(ADC1, &ADC_InitStructure);
		
		/*配置ADC时钟，为PCLK2的6分频，即12MHz*/
		RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
		//配置ADC1的转换顺序和采样周期
		ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);	
		ADC_DMACmd(ADC1,ENABLE);/*按键开启*/
		ADC_Cmd(ADC1, ENABLE);/*初始化的时候一定要使能ADC1，否则ADC初始化会过不去*/
		
		ADC_ResetCalibration(ADC1);/*复位校准寄存器*/		
		while(ADC_GetResetCalibrationStatus(ADC1));/*等待校准寄存器复位完成 */
		ADC_StartCalibration(ADC1);/*ADC校准 */
		while(ADC_GetCalibrationStatus(ADC1));/*等待校准完成*/
		ADC_ExternalTrigConvCmd(ADC1, ENABLE);/*设置ADC外部触发模式*/
}
void ADC1_DMA1_Config()
{
		DMA_InitTypeDef DMA_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		
		/* DMA channel1 配置 */
		DMA_DeInit(DMA1_Channel1);
		DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;/*ADC1的地址*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&(adcval);/*数据存储地址*/
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_BufferSize = 1;/*缓存的数据个数*/
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/*外设地址固定,即ADC->DR寄存器地址固定*/
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;/*数据存储地址固定*/
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;/*半字，2个字节数据传输*/
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;/*半字，2个字节数据传输*/
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;/*单次传输*/
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;/* 指定通道高优先级 */
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;/*非内存到内存模式*/
		DMA_Init(DMA1_Channel1, &DMA_InitStructure);
		/* Enable DMA channel1 */
		DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
		DMA_Cmd(DMA1_Channel1, DISABLE);	//按键开启
		/*enable DMA1 channel 1 NVIC*/
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;/* 抢占优先级0 */
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;/* 子优先级2 */
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
}
/*ADC1初始化*/
void ADC1_Init(void)
{
		ADC1_Config();	//配置ADC
		ADC1_DMA1_Config();	//配置DMA
}
/*ADC1关闭*/
void ADC1Close(void)
{
	  TIM_Cmd(TIM3, DISABLE);/*按键开启*/
    ADC_Cmd(ADC1, DISABLE);/*使能ADC1*/
  	DMA_Cmd(DMA1_Channel1, DISABLE);	//按键开启	
}
/*ADC1关闭*/
void ADC1Open(void)
{
	  TIM_Cmd(TIM3, ENABLE);/*按键开启*/
	  ADC_Cmd(ADC1, ENABLE);/*使能ADC1*/
    DMA_Cmd(DMA1_Channel1, ENABLE);	//按键开启
}
/* 重新配置一次DMA，开启DMA传输 */
void ADCTran(void)
{
		DMA_Cmd(DMA1_Channel1, DISABLE);/*使能DMA1 1通道*/
	  DMA_SetCurrDataCounter(DMA1_Channel1, 160); 
	  DMA_Cmd(DMA1_Channel1, ENABLE);/*使能DMA1 1通道*/
}
