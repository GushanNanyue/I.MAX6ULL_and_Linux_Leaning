#include "i.max6ull.h"

//使能时钟
void cld_enable(void){

    CCM->CCGR0 = 0xFFFFFFFF;
    CCM->CCGR1 = 0xFFFFFFFF;
    CCM->CCGR2 = 0xFFFFFFFF;
    CCM->CCGR3 = 0xFFFFFFFF;
    CCM->CCGR4 = 0xFFFFFFFF;
    CCM->CCGR5 = 0xFFFFFFFF;

}

//初始化led
void gpio_init(void){

    IOMUX_SW_MUX->GPIO1_IO03 = 0x5; //复用为GPIO1_IO03
    IOMUX_SW_PAD->GPIO1_IO03 = 0x10b0; //设置GPIO1_IO03电气属性

    //GPIO初始化
    GPIO1->GDIR = 0x8; //设置为输出
    GPIO1->DR = 0x0; //设置默认打开led

}

//短延时函数
void short_delay(volatile unsigned int n){

    while(n--){

    }
}

//延时函数，在主频396MHz下，一次循环大约1ms
void delay(volatile unsigned int n){

    while (n--)
    {
        short_delay(0x7ff);
    }

}

//开灯
void led_on(void){

    GPIO1->DR &= ~(1<<3);

}

//关灯
void led_off(void){

    GPIO1->DR |= ~(1<<3);

}


int main(void){

    //使能时钟
    cld_enable();

    //初始化GPIO和LED
    gpio_init();

    //设置led0闪烁
    while (1)
    {
       led_on();
       delay(500);

       led_off();
       delay(500);
    }

}