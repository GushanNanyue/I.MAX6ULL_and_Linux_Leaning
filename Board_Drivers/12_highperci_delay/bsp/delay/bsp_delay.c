#include "bsp_delay.h"
#include "bsp_int.h"
#include "bsp_led.h"

//延时初始化函数
void delay_init(void){
	GPT1->CR = 0;
	GPT1->CR = (1 << 15);
	while ((GPT1->CR >> 15) & 0x01);
	//设置GPT1时钟源为ipg_clk = 66MHz.restart模式，默认计数器从0开始
	GPT1->CR |= (1 << 1) | (1 << 6);
	//分频设置
	GPT1->PR = 65; //66分频，频率等于66000000/66 = 1MHz
	//1M的频率计一个数就是一微秒那么0xffffffff就是71.5分钟
	GPT1->OCR[0] = 0xffffffff;
#if 0
	//配置比较通道1
	GPT1->OCR[0] = 1000000/2; //设置中断周期为500ms
	//打开GPT1输出比较通道中断
	GPT1->IR = (1 << 0);
	//使能GIC中断
	GIC_EnableIRQ(GPT1_IRQn);
	//注册中断服务函数
	system_register_irqhandler(GPT1_IRQn,gpt1_irqhandler,NULL);
#endif
	//打开GPT1
	GPT1->CR |= 1 << 0;

}

#if 0
//GPT1中断服务函数
void gpt1_irqhandler(unsigned int gicciar,void* param){
	static unsigned char state = 0;
	if (GPT1->SR & (1 << 0))
	{
		state = !state;
		led_switch(LED0,state);
	}
	GPT1->SR |= (1 << 0);
	

}
#endif

//微秒延时
void delay_us(unsigned int usdelay)
{
	unsigned long oldcnt,newcnt;
	unsigned long tcntvalue = 0;	/* 走过的总时间  */

	oldcnt = GPT1->CNT;
	while(1)
	{
		newcnt = GPT1->CNT;
		if(newcnt != oldcnt)
		{
			if(newcnt > oldcnt)		/* GPT是向上计数器,并且没有溢出 */
				tcntvalue += newcnt - oldcnt;
			else  					/* 发生溢出    */
				tcntvalue += 0XFFFFFFFF-oldcnt + newcnt;
			oldcnt = newcnt;
			if(tcntvalue >= usdelay)/* 延时时间到了 */
			break;			 		/*  跳出 */
		}
	}
}

//毫秒延时
void delay_ms(unsigned int msdelay)
{
	int i = 0;
	for(i=0; i<msdelay; i++)
	{
		delay_us(1000);
	}
}


/*
 * @description	: 短时间延时函数
 * @param - n	: 要延时循环次数(空操作循环次数，模式延时)
 * @return 		: 无
 */
void delay_short(volatile unsigned int n)
{
	while(n--){}
}

/*
 * @description	: 延时函数,在396Mhz的主频下
 * 			  	  延时时间大约为1ms
 * @param - n	: 要延时的ms数
 * @return 		: 无
 */
void delay(volatile unsigned int n)
{
	while(n--)
	{
		delay_short(0x7ff);
	}
}


