/* 
 * 描述: SysTick系统定时器实现
 * 功能: 提供精确的系统时间和延时功能
 */

#include "systick.h"

/* 全局变量 */
static volatile uint32_t SystemTick_ms = 0;  // 系统运行时间(毫秒)
static volatile uint32_t DelayTick_ms = 0;   // 延时计数器

/**
 * @brief  初始化SysTick定时器
 * @note   配置为1ms中断一次
 * @param  无
 * @retval 无
 */
/* systick.c中修改SysTick初始化函数 */
void SysTick_Init(void)
{
    /* SysTick配置为1ms中断一次 */
    /* SystemCoreClock是系统时钟频率(72MHz) */
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        /* 配置失败，停在这里 */
        while (1);
    }
    
    /* 设置SysTick中断优先级 (改为较低优先级) */
    NVIC_SetPriority(SysTick_IRQn, 0x0F);  // 降低SysTick优先级
    
    /* 清零计数器 */
    SystemTick_ms = 0;
    DelayTick_ms = 0;
}

/**
 * @brief  毫秒级延时函数
 * @param  ms: 延时时间(毫秒)
 * @retval 无
 */
void Delay_ms(uint32_t ms)
{
    DelayTick_ms = ms;
    
    /* 等待延时完成 */
    while (DelayTick_ms > 0);
}

/**
 * @brief  微秒级延时函数
 * @note   基于系统时钟的简单延时
 * @param  us: 延时时间(微秒)
 * @retval 无
 */
void Delay_us(uint32_t us)
{
    uint32_t i;
    
    /* 简单的循环延时 */
    /* 72MHz系统时钟，约需要72次循环 = 1us */
    for (i = 0; i < us * 72; i++)
    {
        __NOP();  
    }
}

/**
 * @brief  获取系统运行时间(毫秒)
 * @param  无
 * @retval 系统运行时间(毫秒)
 */
uint32_t GetSysTime_ms(void)
{
    return SystemTick_ms;
}

/**
 * @brief  获取系统运行时间(微秒)
 * @note   精度约为1ms，但返回微秒值
 * @param  无
 * @retval 系统运行时间(微秒)
 */
uint32_t GetSysTime_us(void)
{
    return SystemTick_ms * 1000;
}

/**
 * @brief  SysTick中断处理函数
 * @note   每1ms调用一次
 * @param  无
 * @retval 无
 */
void SysTick_Handler(void)
{
    /* 系统时间递增 */
    SystemTick_ms++;
    
    /* 延时计数器递减 */
    if (DelayTick_ms > 0)
    {
        DelayTick_ms--;
    }
}

/**
 * @brief  SysTick时间递增（兼容接口）
 * @param  无
 * @retval 无
 */
void SysTickIncrement(void)
{
    /* 这个函数保持为空，SysTick_Handler直接处理 */
}
