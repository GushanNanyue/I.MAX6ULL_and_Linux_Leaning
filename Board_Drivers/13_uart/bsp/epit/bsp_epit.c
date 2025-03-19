#include "bsp_epit.h"
#include "bsp_int.h"
#include "bsp_led.h"


//初始化EPIT
void epit1_init(unsigned int frac,unsigned int value){
    if(frac > 0XFFF)
		frac = 0XFFF;

    //配置EPIT_CR寄存器
     EPIT1->CR = 0;
    // //设置寄存器的初始值为加载寄存器
    // EPIT1->CR = (1 << 1);
    // //使能比较中断
    // EPIT1->CR = (1 << 2);
    // //选择工作模式为iset_and_forget
    // EPIT1->CR = (1 << 3);
    // //设置分频值
    // EPIT1->CR = (frac << 4);
    // //选择时钟源
    // EPIT1->CR = (1 << 24);
    EPIT1->CR = (1<<24 | frac << 4 | 1<<3 | 1<<2 | 1<<1);


    //设置加载值(倒计时的值)
    EPIT1->LR = value;
    //设置比较寄存器
    EPIT1->CMPR = 0;
    //初始化中断
    GIC_EnableIRQ(EPIT1_IRQn);

    system_register_irqhandler(EPIT1_IRQn,(system_irq_handler_t)epit1_irqhandler,NULL);

    //打开EPIT1
    EPIT1->CR |= 1 << 0;
}

    //EPIT1中断服务函数
void epit1_irqhandler(unsigned int gicciar,void* param){
    static unsigned char state = 0;

    state = !state;
    //判断比较中断是否发生
    if (EPIT1->SR & (1  << 0))
    {
        led_switch(LED0,state);
    }

    //和中断完成后清除标志位
    EPIT1->SR |= (1 << 0);


    

}


