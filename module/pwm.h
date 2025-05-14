/* 
 * 描述: PWM模块头文件
 * 功能: 声明PWM相关函数
 */

#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

/* 函数声明 */
void PWM_Config(void);                       // 配置PWM
void PWM_UpdateBreathingEffect(void);        // 更新呼吸灯效果
void PWM_SetBreathingEffect(uint8_t enable); // 设置呼吸灯效果

/* 外部函数声明 */
extern uint32_t GetSysTime_ms(void);         // 获取系统时间

#endif /* __PWM_H */
