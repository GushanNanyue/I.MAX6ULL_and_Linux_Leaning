#include "bsp_led.h"
#include "cc.h"

//初始化led
void led_init(void){

    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO03_GPIO1_IO03,0); //复用为GPIO1_IO03
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO03_GPIO1_IO03,0x10B0); //设置GPIO1_IO03电气属性

    //GPIO初始化
    GPIO1->GDIR = 0x8; //设置为输出
    GPIO1->DR = 0x0; //设置默认打开led

}


//开灯
void led_on(void){

    GPIO1->DR &= ~(1<<3);

}

//关灯
void led_off(void){

    GPIO1->DR |= ~(1<<3);

}

void led_switch(int led,int status){

    switch (status)
    {
    case LED0:
        if(status == ON)
            GPIO1->DR &= ~(1<<3);
        else if(status == OFF)
            GPIO1->DR |= ~(1<<3);
        break;
    
    default:
        break;
    }

}