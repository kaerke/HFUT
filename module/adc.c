/* 
 * 描述: ADC模块，用于温度采集
 * 功能: 配置ADC，读取LM35传感器数据并转换为温度值
 */

#include "stm32f10x.h"
#include "adc.h"

/* ADC采样结果变量 */
__IO uint16_t ADC_ConvertedValue = 0;

/**
 * @brief  配置ADC模块
 * @param  无
 * @retval 无
 */
void ADC_Config(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* 使能ADC1和GPIOA时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    /* 配置PA2为模拟输入 - LM35连接到此引脚 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* ADC1配置 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    /* 配置ADC1通道2为55.5个采样周期，采样率约50Hz */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_55Cycles5);
    
    /* 使能ADC1 */
    ADC_Cmd(ADC1, ENABLE);
    
    /* 校准ADC */
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    
    /* 启动ADC1转换 */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
 * @brief  获取ADC转换结果
 * @param  无
 * @retval 原始ADC值
 */
uint16_t ADC_GetValue(void)
{
    /* 等待转换完成 */
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    
    /* 返回ADC1转换结果 */
    return ADC_GetConversionValue(ADC1);
}

/**
 * @brief  获取当前温度值
 * @param  无
 * @retval 温度值(摄氏度)
 */
float ADC_GetTemperature(void)
{
    uint16_t adcValue;
    float voltage, temperature;
    
    /* 获取ADC值 */
    adcValue = ADC_GetValue();
    
    /* ADC值转换为电压: 0-4095 -> 0-3.3V */
    voltage = (float)adcValue * 3.3f / 4095.0f;
    
    /* LM35转换: 每10mV代表1摄氏度 */
    temperature = voltage * 100.0f;
    
    return temperature;
}
