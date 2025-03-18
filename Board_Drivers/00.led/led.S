
.global _start @全局标签

_start:
    /*打开外设所需要的时钟 */
    ldr r0, =0x020C406C    @CCM_CCGR1的地址
    ldr r1, =0xffffffff    @全部设置为11,打开时钟
    str r1, [r0]           @将R1中的数据写入到R0中存储的地址中去

    /*配置IO复用功能 */
    ldr r0, =0x20E0040    @IOMUXC_SW_MUX_CTL_PAD_SNVS_TAMPER9 复用寄存器地址
    ldr r1, =0x05         @设置为GPIO功能
    str r1, [r0]          @将R1中的数据写入到R0中存储的地址中去

    /*配置IO的电气属性 */
    ldr r0, =0x20E02CC    @IOMUXC_SW_PAD_CTL_PAD_SNVS_TAMPER9 电气属性寄存器地址
    ldr r1, =0x10B0         @设置为GPIO功能 0000000010110000
    str r1, [r0]          @将R1中的数据写入到R0中存储的地址中去

    /*设置IO方向 */
    ldr r0, =0x20AC004    @GPIO5_GDIR的地址
    ldr r1, =0x200        @GPIO5_IO9设置为输出
    str r1, [r0]          @将R1中的数据写入到R0中存储的地址中去

    /*设置IO的电平 */
    ldr r0, =0x20AC000    @GPIO5_DR的地址
    ldr r1, =0            @GPIO5_IO9设置为输出为低电平
    str r1, [r0]          @将R1中的数据写入到R0中存储的地址中去

loop:
    b loop
    



