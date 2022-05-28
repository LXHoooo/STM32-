#include "DAC.h"
#include "ADC.h"

#define DAC_DHR12RD_Address     0x40007420
/*
 * 函数名：DAC1_GPIO_Config
 * 描述  ：使能DAC1和DMA1的时钟，初始化PA.04
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
*/
void DAC1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	  /* 使能 GPIOA 和 DAC 时钟*/
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC|RCC_APB1Periph_TIM2, ENABLE);//注意DAC clock is int APB1
	
	  /* 配置PA.04为模拟输出 */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	//GPIO_Pin_4|,//PA4，PA5是DAC1,DAC2
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);			
	  /* TIME1 configuration */
	  TIM_DeInit(TIM2);
	  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	  /* TIM3 used for timing, the timing period depends on the sample rate */
	  TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;    /* TIM2CLK = 72 MHz */
	  TIM_TimeBaseStructure.TIM_Period = 124;             //对应8K采样
	  TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;
	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	  TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_Update); //选择TRGO触发源为计时器更新时间
    TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE); //使能指定的TIM3中断,允许更新中
	  TIM_Cmd(TIM2, DISABLE);
		//中断优先级NVIC设置
	  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
   	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	  NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	  /* 配置DAC1 */
	  DAC_DeInit();
	  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;	// DAC_Trigger_T2_TRGO
	  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None ; 	 //无噪声
	  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;		//使能输出，不打开就没有输出
	  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	  DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12位右对齐数据格式设置DAC值
	  DAC_DMACmd(DAC_Channel_1, ENABLE);/*使能DAC指定的DMA通道请求*/
	  DAC_Cmd(DAC_Channel_1,DISABLE);
}
/*DAC的DMA初始化*/
void DAC1_DMA2_Config(void)
{
	  DMA_InitTypeDef  DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);/*使能DMA2时钟*/
	
	  DMA_DeInit(DMA2_Channel3);/*将DMA的通道13寄存器重设为默认值*/
	  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_Address;/*定义DMA外设基地址*/
	  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&(TrueDACBuff);/*用于定义DMA内存基地址*/
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;/*设置外设作为数据传输的目的地*/
	  DMA_InitStructure.DMA_BufferSize = VcBuf_Len;/*定义DMA缓存的大小，单位为数据单位，等于DMA_PeripheralDataSize或DMA_MemoryDataSize的值*/										
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/*设置外设地址不变，就一直是DAC_DHR12RD这个寄存器*/
  	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;/*内存地址寄存器递增*/
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;/*设定了外设数据宽度，16位*/
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;/*设定了外设数据宽度为16位*/	
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;/*工作在循环缓存模式*/
	  DMA_InitStructure.DMA_Priority = DMA_Priority_High;/*该DMA通道拥有高优先级*/
	  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;/*非内存到内存传输*/
	  DMA_Init(DMA2_Channel3, &DMA_InitStructure);

	  DMA_ITConfig(DMA2_Channel3, DMA_IT_TC, ENABLE);/*DMA通道中断使能*/
	  DMA_Cmd(DMA2_Channel3, DISABLE);/*使能DMA2 3通道*/
	  /*enable DMA1 channel 1 NVIC*/
	  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel3_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;/* 抢占优先级0 */
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;/* 子优先级2 */
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
}
/*DAC初始化*/
void DAC1_Init(void)
{
	  DAC1_Config();
    DAC1_DMA2_Config();
}
/*DAC开启*/
void DAC1Open(void)
{	
	  TIM_Cmd(TIM2, ENABLE);/*使能DAC指定的DMA通道请求*/
	  DAC_Cmd(DAC_Channel_1,ENABLE);
	  DMA_Cmd(DMA2_Channel3, ENABLE);/*使能DMA2 3通道*/
}
/*DAC关闭*/
void DAC1Close(void)
{
	  TIM_Cmd(TIM2, DISABLE);/*使能DAC指定的DMA通道请求*/
	  DAC_Cmd(DAC_Channel_1,DISABLE);
	  DMA_Cmd(DMA2_Channel3, DISABLE);/*使能DMA2 3通道*/
}
/* 重新配置一次DMA，开启DMA传输 */
void DACTran(void)
{
		DMA_Cmd(DMA2_Channel3, DISABLE);/*使能DMA2 3通道*/
	  DMA_SetCurrDataCounter(DMA2_Channel3, 160); 
	  DMA_Cmd(DMA2_Channel3, ENABLE);/*使能DMA2 3通道*/
}
