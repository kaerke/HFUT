/* 
 * 描述: ADC模块头文件
 * 功能: 声明ADC相关函数
 */

#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

/* 函数声明 */
void ADC_Config(void);              // 配置ADC
uint16_t ADC_GetValue(void);        // 获取ADC转换结果
float ADC_GetTemperature(void);     // 获取温度值

#endif /* __ADC_H */
