.syntax unified
.arm

.global _start

_start:
    //设置处理器禁用SVC模式
    mrs r0, cpsr    //读取cpsr
    bic r0, r0, #0x1f   //其清除cpsr后五位
    orr r0, r0, #0x13   //使用SVC模式
    msr cpsr, r0    //将r0写入cpsr

    //清除BSS段
    ldr r0, =_bss_start
    ldr r1, =_bss_end
    mov r2, #0
bss_loop:
    stmia r0! , {r2}
    cmp r0, r1
    ble bss_loop

    //设置SP指针
    ldr sp, =0x80200000 //设置sp指针
    b main  //跳转到c语言main函数


.global _bss_start
_bss_start:
    .word __bss_start

.global _bss_end
_bss_end:
    .word __bss_end