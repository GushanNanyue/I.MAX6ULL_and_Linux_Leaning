#include "main.h"

//使能时钟
void cld_enable(void){

    CCCM_CCGR1 = 0xFFFFFFFF;
    CCCM_CCGR2 = 0xFFFFFFFF;
    CCCM_CCGR3 = 0xFFFFFFFF;
    CCCM_CCGR4 = 0xFFFFFFFF;
    CCCM_CCGR5 = 0xFFFFFFFF;
    CCCM_CCGR6 = 0xFFFFFFFF;

}

//初始化led
void gpio_init(void){

    SW_MUX_CPIO_IO03 = 0x5; //复用为GPIO1_IO03
    SW_PAD_CPIO_IO03 = 0x10b0; //设置GPIO1_IO03电气属性

    //GPIO初始化
    GPIO1_GDIR = 0x8; //设置为输出
    GPIO1_DR = 0x0; //设置默认打开led

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

    GPIO1_DR &= ~(1<<3);

}

//关灯
void led_off(void){

    GPIO1_DR |= ~(1<<3);

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