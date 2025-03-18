#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "bsp_key.h"
#include "bsp_int.h"

int main(void)
{
	int_init();			//中断初始化
	imx6u_clkinit();	//初始化系统时钟
	clk_enable();		/* 使能所有的时钟 			*/
	led_init();			/* 初始化led 			*/
	beep_init();		/* 初始化beep	 		*/
	key_init();			//初始化按键

	int i = 0;
	unsigned char led_state = OFF;
	unsigned char beep_state = OFF;
	int key_value = 0;

	while(1)			
	{	
		//按键控制蜂鸣器
		key_value = key_getvalue();
		if (key_value)
		{
			switch (key_value)
			{
			case KEY0_VALUE:
				beep_state = !beep_state;
				beep_switch(beep_state);
				break;
			
			default:
				break;
			}
		}
		


		//闪烁提示运行
		i++;
		if(i == 50){
			i = 0;
			led_state = !led_state;
			led_switch(LED0,led_state);
		}
		delay(10);
	}

	return 0;
}
