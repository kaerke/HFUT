/* 
 * 文件名: pwm.c (优化版本)
 * 描述: PWM模块，用于2Hz频率的精确呼吸灯效果
 * 功能: 配置定时器产生PWM信号，实现2Hz LED呼吸效果
 */

#include "stm32f10x.h"
#include "pwm.h"
#include "systick.h"  // 使用精确时间系统

/* 定义呼吸灯参数 */
#define PWM_PERIOD      1000    // PWM周期值
#define BREATHING_FREQ  2       // 呼吸频率 2Hz
#define BREATHING_STEPS 50      // 一个呼吸周期的步数
#define STEP_TIME_MS    (500 / BREATHING_STEPS)  // 每步的时间间隔 (500ms / 50steps = 10ms)

/* 呼吸灯相关变量 */
uint8_t breathing_enabled = 0;      // 呼吸灯使能标志
uint16_t pwm_value = 0;             // 当前PWM值
uint8_t brightness_up = 1;          // 亮度增加标志
uint8_t breathing_step = 0;         // 当前呼吸步数

/**
 * @brief  配置PWM模块
 * @param  无
 * @retval 无
 */
void PWM_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    /* 使能定时器和GPIO时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  // PA1对应TIM2_CH2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    
    /* 配置PA1为复用推挽输出 - 红色LED连接到此引脚 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出，支持PWM功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 配置定时器基本参数 */
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD - 1;  // 1000-1
    TIM_TimeBaseStructure.TIM_Prescaler = 720 - 1;      // 72MHz / 720 = 100kHz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    /* 配置PWM通道参数 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0; // 初始占空比0%
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);  // PA1对应TIM2_CH2
    
    /* 使能PWM输出 */
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    
    /* 启动定时器 */
    TIM_Cmd(TIM2, ENABLE);
    
    /* 确保PWM初始值为0 */
    pwm_value = 0;
    TIM_SetCompare2(TIM2, pwm_value);
}

/**
 * @brief  更新PWM值以实现2Hz精确呼吸灯效果
 * @param  无
 * @retval 无
 */
void PWM_UpdateBreathingEffect(void)
{
    static uint32_t last_update_time = 0;
    uint32_t current_time;
    
    /* 如果呼吸灯未使能，直接返回 */
    if (!breathing_enabled)
    {
        /* 确保PWM输出为0 */
        TIM_SetCompare2(TIM2, 0);
        return;
    }
    
    /* 获取当前精确系统时间 */
    current_time = GetSysTime_ms();
    
    /* 每STEP_TIME_MS(10ms)更新一次PWM值，确保2Hz呼吸频率 */
    if (current_time - last_update_time >= STEP_TIME_MS)
    {
        /* 更新PWM值 */
        if (brightness_up)
        {
            /* 亮度递增 */
            pwm_value = (breathing_step * PWM_PERIOD) / BREATHING_STEPS;
            breathing_step++;
            
            if (breathing_step >= BREATHING_STEPS)
            {
                breathing_step = BREATHING_STEPS;
                brightness_up = 0;
            }
        }
        else
        {
            /* 亮度递减 */
            breathing_step--;
            pwm_value = (breathing_step * PWM_PERIOD) / BREATHING_STEPS;
            
            if (breathing_step == 0)
            {
                brightness_up = 1;
            }
        }
        
        /* 确保PWM值在有效范围内 */
        if (pwm_value > PWM_PERIOD)
            pwm_value = PWM_PERIOD;
            
        /* 更新PWM占空比 */
        TIM_SetCompare2(TIM2, pwm_value);
        
        /* 更新上次更新时间 */
        last_update_time = current_time;
    }
}

/**
 * @brief  设置呼吸灯效果
 * @param  enable: 0 - 关闭呼吸灯，1 - 启动呼吸灯
 * @retval 无
 */
void PWM_SetBreathingEffect(uint8_t enable)
{
    if (enable)
    {
        // 如果当前呼吸效果未启动，则进行初始化设置
        if (!breathing_enabled)
        {
            /* 确保PA1配置为AF功能 */
            GPIO_InitTypeDef GPIO_InitStructure;
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(GPIOA, &GPIO_InitStructure);
            
            /* 初始化呼吸参数 */
            pwm_value = 0;
            brightness_up = 1;
            breathing_step = 0;
            
            /* 立即更新PWM值 */
            TIM_SetCompare2(TIM2, pwm_value);
        }
        
        /* 启动呼吸灯效果 */
        breathing_enabled = 1;
        
        /* 使能定时器确保PWM输出 */
        TIM_Cmd(TIM2, ENABLE);
    }
    else
    {
        /* 关闭呼吸灯效果 */
        breathing_enabled = 0;
        
        /* 重置呼吸参数 */
        pwm_value = 0;
        breathing_step = 0;
        brightness_up = 1;
        
        /* 确保PWM输出为0 */
        TIM_SetCompare2(TIM2, 0);
    }
}
