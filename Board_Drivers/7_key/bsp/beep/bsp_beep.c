#include "bsp_beep.h"
#include "bsp_gpio.h"
#include "cc.h"

void beep_init(void)
{
	gpio_pin_config_t beep_config;
	/* 1、初始化IO复用，复用为GPIO5_IO01 */
	IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01,0);		
	
	
	/* 2、、配置GPIO1_IO03的IO属性	
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 00 默认下拉
	 *bit [13]: 0 kepper功能
	 *bit [12]: 1 pull/keeper使能
	 *bit [11]: 0 关闭开路输出
	 *bit [7:6]: 10 速度100Mhz
	 *bit [5:3]: 110 R0/6驱动能力
	 *bit [0]: 0 低转换率
	 */
	IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01,0X10B0);
	
	beep_config.direction = kGPIO_DigitalOutPut;
	beep_config.outputLogic = 1;
	gpio_init(GPIO5,1,&beep_config);
	
}

/*
 * @description		: 蜂鸣器控制函数，控制蜂鸣器打开还是关闭
 * @param - status	: 0，关闭蜂鸣器，1 打开蜂鸣器
 * @return 			: 无
 */
void beep_switch(int status)
{	
	if(status == ON)
		gpio_pinwrite(GPIO5,1,0);	/* 打开蜂鸣器 */
	else if(status == OFF)
		gpio_pinwrite(GPIO5,1,1);	/* 关闭蜂鸣器 */
}
