#include "bsp_key.h"
#include "bsp_delay.h"
#include "bsp_gpio.h"

//初始化按键
void key_init(void){

    gpio_pin_config_t key_config;
    /* 1、初始化IO复用，复用为GPIO1_IO18 */
	IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18,0);		
	IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18,0XF080);
	//GPIO初始化
    key_config.direction = kGPIO_DigitalInput;
	gpio_init(GPIO1,18,&key_config); 
		
}

//读取按键值:返回0为按下，1为未按下
int read_key(void){

    int ret = 0;
    ret = gpio_pinread(GPIO1,18);
    return ret;
}

//获得按键值并消抖
int key_getvalue(void){
    int ret = 0;
    static unsigned char release = 1; //为1表示按键被松开
    if ((release == 1) && (read_key() == 0)) //两边条件同时满足则是按下且触发抖动
    {
        delay(10);
        release = 0;
        if(read_key() == 0) //延时后read_key函数还为0则按键有效
        {
            ret = KEY0_VALUE;
        }
       
    }
     else if (read_key() == 1)
    {
        ret = KEY_NONE;
        release = 1;
    }
    return ret;
}