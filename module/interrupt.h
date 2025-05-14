/* 
 * 文件名: interrupt.h
 * 描述: 中断模块头文件
 * 功能: 声明中断相关函数
 */

#ifndef __INTERRUPT_H
#define __INTERRUPT_H

#include "stm32f10x.h"

/* 函数声明 */
void GPIO_Config(void);    // 配置GPIO引脚
void EXTI_Config(void);    // 配置外部中断
void uesr_SystemInit(void);     // 系统初始化

#endif /* __INTERRUPT_H */
