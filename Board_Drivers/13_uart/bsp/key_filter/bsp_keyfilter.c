#include "bsp_keyfilter.h"
#include "bsp_gpio.h"
#include "bsp_int.h"
#include "bsp_beep.h"

//keyfilter初始化
void keyfilter_init(void){
    gpio_pin_config_t key_config;
    //复用为GPIO01_IO18
    IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18,0);
    IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18,0xF080);

    //GPIO初始化
    key_config.direction = kGPIO_DigitalInput;
    key_config.interruptMode = kGPIO_IntFallingEdge;
    key_config.outputLogic = 1;
    gpio_init(GPIO1,18,&key_config);

    GIC_EnableIRQ(GPIO1_Combined_16_31_IRQn);
    system_register_irqhandler(GPIO1_Combined_16_31_IRQn,(system_irq_handler_t)gpio1_16_31_irqhandler,NULL);
    
    gpio_enableint(GPIO1,18);
    //初始化计时器
    filtertimer_init(66000000/100);
}

//初始化EPIT1定时器
void filtertimer_init(unsigned int value){

    EPIT1->CR = 0;
    EPIT1->CR = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 24);
    //设置加载值(倒计时的值)
    EPIT1->LR = value;
    //设置比较寄存器
    EPIT1->CMPR = 0;
    //初始化中断
    GIC_EnableIRQ(EPIT1_IRQn);
    system_register_irqhandler(EPIT1_IRQn,(system_irq_handler_t)filtertimer_irqhandler,NULL);
      
}

//重启EPIT1计时器
void restart_filtertimer(unsigned int value){
    //关闭EPIT1计时器
    EPIT1->CR &= ~(1 << 0);
    //设置多久重启
    EPIT1->LR = value;
    EPIT1->CR |= 1 << 0;

}

//EPIT1定时器中断处理函数
void filtertimer_irqhandler(unsigned int gicciar,void* param){

    static unsigned char state = OFF;
    if(EPIT1->SR & (1 << 0)){
        //关闭定时器
        EPIT1->CR &= ~(1 << 0);
        if (gpio_pinread(GPIO1,18) == 0)
        {
            state = !state;
            beep_switch(state);
        }
        

    }
    //清除中断标志
    EPIT1->SR |= (1 << 0);
}

//按键中断服务函数
void gpio1_16_31_irqhandler(unsigned int gicciar, void *param){

    //开启定时器
    restart_filtertimer(66000000/100);
    //清除中断标志位
    gpio_clearintflags(GPIO1,18);

}