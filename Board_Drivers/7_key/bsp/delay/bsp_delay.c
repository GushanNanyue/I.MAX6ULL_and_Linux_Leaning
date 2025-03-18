#include "bsp_delay.h"

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

