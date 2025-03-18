#include "bsp_clk.h"


//使能时钟
void clk_enable(void){

    CCM->CCGR0 = 0xFFFFFFFF;
    CCM->CCGR1 = 0xFFFFFFFF;
    CCM->CCGR2 = 0xFFFFFFFF;
    CCM->CCGR3 = 0xFFFFFFFF;
    CCM->CCGR4 = 0xFFFFFFFF;
    CCM->CCGR5 = 0xFFFFFFFF;

}

//初始化时钟
void imx6u_clkinit(void){

    unsigned int reg = 0;


    //设置PLL1
    //初始化主频为528HZ
    if (((CCM->CCSR >> 2) & 0x1) == 0 ) //当前时钟使用pll1_main_clk也就是PLL1
    {
        //切换备用时钟（切换到晶振）用以在修改主频时不出现问题
        CCM->CCSR &= ~(1 << 8); //设置step_clk = osc_clk =24MHz
        CCM->CCSR |= (1 << 2); //设置pll1_sw_clk使用step_clk
    }

    //设置pll1 = 1056MHz
    CCM_ANALOG->PLL_ARM = (1 << 13) | ((88 << 0) & 0x7F);
    //设置2分频
    CCM->CACRR = 1;
    //将pll1_sw_clk切换回pll1_main_clk = 528MHz
    CCM->CCSR &= ~(1 << 2);
    

    //设置PLL2
    //设置PLL2的四路PFD
    reg = CCM_ANALOG->PFD_528;
    reg &= ~(0x3F3F3F3F);
    //PLL2_PFD3 = 297MHz
    reg |= (32 << 24);
    //PLL2_PFD2 = 396MHz
    reg |= (24 << 16);
    //PLL2_PFD1 = 594MHz
    reg |= (16 << 8);
    //PLL2_PFD0 = 352MHz
    reg |= (27 << 0);

    CCM_ANALOG->PFD_528 = reg;


    //设置PLL3
    //设置PLL3的四路PFD
    reg = 0;
    reg = CCM_ANALOG->PFD_480;
    reg &= ~(0x3F3F3F3F);
    //PLL3_PFD3 = 454.7MHz
    reg |= (19 << 24);
    //PLL3_PFD2 = 508.2MHz
    reg |= (17 << 16);
    //PLL3_PFD1 = 540MHz
    reg |= (16 << 8);
    //PLL3_PFD0 = 720MHz
    reg |= (12 << 0);

    CCM_ANALOG->PFD_528 = reg;


    //其他时钟源
    //AHB_CLK_ROOT = 132MHz
    //设置PRE_PERIPH_CLOCK = PLL2_PFD2 = 396MHz
    CCM->CBCMR &= ~(3 << 18);
    CCM->CBCMR |= (1 << 18);  
    CCM->CBCDR &= ~(1 << 25);
    //等待握手信号
    while (CCM->CDHIPR & (1 << 5));

    reg = 0;
    reg = CCM->CBCDR;
    reg &= ~(7 << 10);
    //3分频
    reg |= (2 << 10);
    CCM->CBCDR = reg;
    //等待握手信号
    while (CCM->CDHIPR & (1 << 1));
   
    //IPG_CLK_ROOT = 66MHz
    CCM->CBCDR &= ~(3 << 8);
    CCM->CBCDR |= (1 << 8);//IPG_CLK_ROOT = AHB_CLK_ROOT / 2 =132 / 2 = 66MHz

    //PER_CLK_CLK = 66MHz
    CCM->CSCMR1 &= ~(1 << 6);//PERCLK_CL_ROOT时钟源为IPG_CLK = 66MHz
    CCM->CSCMR1 &= ~(0x3F << 0); //1分频，PERCLK_CLK_ROOT = 66MHz
    

}


