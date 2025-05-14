/* 
 * 描述: 中断模块
 * 功能: 配置外部中断，实现按键控制功能
 */

#include "stm32f10x.h"
#include "interrupt.h"

/**
 * @brief  配置GPIO引脚
 * @param  无
 * @retval 无
 */
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* 使能GPIOA时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    /* 配置PA0为推挽输出 - 绿色LED */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    
    /* 配置PA8为上拉输入 - 按键 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 初始化LED状态 */
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
}

/**
 * @brief  配置外部中断
 * @param  无
 * @retval 无
 */
void EXTI_Config(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* 使能AFIO时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    /* 连接EXTI线到PA8 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
    
    /* 配置EXTI8线 */
    EXTI_InitStructure.EXTI_Line = EXTI_Line8;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    /* 配置中断优先级 */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; // EXTI8在EXTI9_5_IRQn中
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  系统初始化
 * @param  无
 * @retval 无
 */
void uesr_SystemInit(void)
{
    /* 重置所有外设 */
    RCC_DeInit();
    
    /* 配置系统时钟 */
    RCC_HSEConfig(RCC_HSE_ON);  // 使能外部高速时钟
    
    /* 等待HSE稳定 */
    if(RCC_WaitForHSEStartUp() == SUCCESS)
    {
        /* 配置PLL */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);  // 8MHz * 9 = 72MHz
        
        /* 使能PLL */
        RCC_PLLCmd(ENABLE);
        
        /* 等待PLL稳定 */
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
        
        /* 设置系统时钟源为PLL */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        
        /* 等待系统时钟切换到PLL */
        while(RCC_GetSYSCLKSource() != 0x08);
    }
    
    /* 配置AHB、APB1、APB2时钟 */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);      // AHB时钟 = SYSCLK = 72MHz
    RCC_PCLK1Config(RCC_HCLK_Div2);       // APB1时钟 = HCLK/2 = 36MHz
    RCC_PCLK2Config(RCC_HCLK_Div1);       // APB2时钟 = HCLK = 72MHz
    
    /* 配置Flash访问时间 */
    FLASH_SetLatency(FLASH_Latency_2);
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
}
