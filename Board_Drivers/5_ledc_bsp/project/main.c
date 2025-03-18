#include "main.h"
#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"

int main(void){

    //使能时钟
    cld_enable();

    //初始化GPIO和LED
    gpio_init();

    //设置led0闪烁
    while (1)
    {
       /* 打开LED0 */
        led_switch(LED0,ON);		
		delay(500);

		/* 关闭LED0 */
		led_switch(LED0,OFF);	
		delay(500);
    }

    return 0;

}