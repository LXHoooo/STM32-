#include "common.h"

/*主程序*/
int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	delay_init();
	uart_init(9600);	 	//串口初始化为9600	
	ADC1_Init();
	DAC1_Init();
	KEY_Init();
	LED_Init();
	Speex_Init();
	NRF24L01_Init();    	//初始化NRF24L01  
	LCD_Init();			   	//初始化LCD 	
	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(60,50,200,16,16,"Mini STM32");	
	LCD_ShowString(60,70,200,16,16,"Wireless intercon");	
	LCD_ShowString(60,90,200,16,16,"LXH");
	LCD_ShowString(60,110,200,16,16,"2021/11/4");		
	while(NRF24L01_Check())	//检查NRF24L01是否在位，不在位灯光报警
	{
		LCD_ShowString(60,130,200,16,16,"NRF24L01 Error");
		delay_ms(200);
		LCD_Fill(60,130,239,130+16,WHITE);
 		delay_ms(200);
	}	
  LCD_ShowString(60,130,200,16,16,"NRF24L01 OK");
	while(1)
	{
		key = KEY_Scan(0);
    switch(key)
		{
		    case 1:
					key = 0;
          NRF24L01_TX_Mode();
				  LCD_ShowString(60,170,239,32,16,"Sended DATA:");
					ADC1Open();
				  DAC1Close();
				  LED0 = 0;
				break;
				case 2:
					key = 0;
				  NRF24L01_RX_Mode();
			   	LCD_ShowString(60,170,239,32,16,"Receive DATA:");
				  ADC1Close();
				  DAC1Open();
					LED1 = 0;
				break;
				default:
				break;
		}
	}
}
/**
  * @brief  Ovveride the _speex_putc function of the speex library
  * @param  None
  * @retval : None
*/
void _speex_putc(int ch, void *file)
{
  while(1)
  {
  };
}
/**
  * @brief  Ovveride the _speex_fatal function of the speex library
  * @param  None
  * @retval : None
  */
void _speex_fatal(const char *str, const char *file, int line)
{
  while(1)
  {
  };
}
