/* 
 * 描述: SysTick系统定时器头文件
 * 功能: 提供精确的系统时间和延时功能
 */

#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"

/* 函数声明 */
void SysTick_Init(void);           // 初始化SysTick
void Delay_ms(uint32_t ms);        // 毫秒级延时
void Delay_us(uint32_t us);        // 微秒级延时
uint32_t GetSysTime_ms(void);      // 获取系统运行时间(毫秒)
uint32_t GetSysTime_us(void);      // 获取系统运行时间(微秒)
void SysTickIncrement(void);       // SysTick中断调用的时间递增函数

#endif /* __SYSTICK_H */
