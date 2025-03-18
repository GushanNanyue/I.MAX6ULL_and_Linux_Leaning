//全局标号
.global _start 

//程序从此函数开始完成时钟使能、
//GPIO初始化、最终控制GPIO输出低电平来点亮LED灯
_start:
    ldr r0, =0x020c4068     
    ldr r1, =0xffffffff
    str r1,[r0]

    ldr r0, =0x020c406c
    str r1,[r0]

    ldr r0, =0x020c4070
    str r1,[r0]

    ldr r0, =0x020c4074
    str r1,[r0]

    ldr r0, =0x020c4078
    str r1,[r0]

    ldr r0, =0x020c407c
    str r1,[r0]

    ldr r0, =0x020c4080
    str r1,[r0]

//设置寄存器GPIO_IO03的复用
//复用为GPIO_io03
    ldr r0, =0x020e0068
    ldr r1, =0x5
    str r1,[r0]

//配置GPIO_IO03的IO属性
/* *bit 16:0 HYS关闭 
*bit [15:14]: 00 默认下拉 
*bit [13]: 0 kepper功能 
*bit [12]: 1 pull/keeper使能 
*bit [11]: 0 关闭开路输出 
*bit [7:6]: 10 速度100Mhz 
*bit [5:3]: 110 R0/6驱动能力 
*bit [0]: 0 低转换率 */
    ldr r0, =0x020e02f4
    ldr r1, =0x10b0
    str r1,[r0]

    ldr r0, =0x0209c004
    ldr r1, =0x0000008
    str r1,[r0]

    ldr r0, =0x0209c000
    ldr r1, =0
    str r1,[r0]
//loop死讯环
loop:
    b loop

