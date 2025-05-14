/*
 * 描述: STM32F103温度监测系统主程序 - 使用精确的SysTick时间系统
 * 功能: 采集LM35温度数据、通过串口通信、LED状态指示和按键控制温度阈值
 */

#include "stm32f10x.h"
#include "adc.h"
#include "pwm.h"
#include "usart.h"
#include "interrupt.h"
#include "systick.h"  
#include <stdio.h>
#include <string.h>

/* 定义全局变量 */
float current_temp = 0.0f;           // 当前温度值
uint8_t temp_threshold_index = 1;    // 温度阈值索引，默认使用第二个阈值(30度)
const float temp_thresholds[3] = {25.0f, 30.0f, 35.0f}; // 三档温度阈值
float current_threshold = 30.0f;     // 当前温度阈值，默认30度
uint8_t system_init_complete = 0;    // 系统初始化完成标志
uint8_t serial_rx_flag = 0;          // 串口接收标志
uint8_t serial_rx_data = 0;          // 串口接收的数据


void SystemInit(void);               // 系统初始化
void LED_Control(void);              // LED控制函数
void Process_Serial_Command(void);   // 处理串口命令
void Send_Temperature(void);         // 发送温度数据
void Check_Temperature(void);        // 检测温度并更新LED状态

/* 主函数 */
int main(void)
{
    /* 系统初始化 */
    uesr_SystemInit();
    
    /* 初始化SysTick精确时间系统 */
    SysTick_Init();
    
    /* 各模块初始化 */
    ADC_Config();    // 配置ADC，50Hz采样率
    PWM_Config();    // 配置PWM，用于呼吸灯效果
    USART_Config();  // 配置串口，波特率9600
    GPIO_Config();   // 配置GPIO
    EXTI_Config();   // 配置外部中断
    
    /* 系统启动指示：绿灯闪烁2次 */
    GPIO_SetBits(GPIOA, GPIO_Pin_0);    // 绿灯亮
    Delay_ms(300);                      // 精确延时300ms
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);  // 绿灯灭
    Delay_ms(300);                      // 精确延时300ms
    GPIO_SetBits(GPIOA, GPIO_Pin_0);    // 绿灯亮
    Delay_ms(300);                      // 精确延时300ms
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);  // 绿灯灭
    Delay_ms(300);                      // 精确延时300ms
    
    /* 绿灯常亮，表示系统工作正常 */
    GPIO_SetBits(GPIOA, GPIO_Pin_0);
    
    /* 标记系统初始化完成 */
    system_init_complete = 1;
    
    /* 主循环 */
    while (1)
    {
        /* 采集温度数据 */
        current_temp = ADC_GetTemperature();
        
        /* 检测温度并更新LED状态 */
        Check_Temperature();
        
        /* 更新呼吸灯效果 */
        PWM_UpdateBreathingEffect();
        
        /* 定时发送温度数据 */
        Send_Temperature();
        
        /* 处理串口接收到的命令 */
        if (serial_rx_flag)
        {
            Process_Serial_Command();
            serial_rx_flag = 0;
        }
        
        /* 精确延时20ms，控制主循环频率50Hz */
        Delay_ms(20);
    }
}

/* 检测温度并更新LED状态 */
void Check_Temperature(void)
{
    if (current_temp > current_threshold)
    {
        /* 超温报警：绿灯灭，红灯呼吸效果 */
        GPIO_ResetBits(GPIOA, GPIO_Pin_0);  // 绿灯灭
        PWM_SetBreathingEffect(1);          // 启动红灯呼吸效果
    }
    else
    {
        /* 温度正常：绿灯亮，红灯灭 */
        GPIO_SetBits(GPIOA, GPIO_Pin_0);    // 绿灯亮
        PWM_SetBreathingEffect(0);          // 关闭红灯呼吸效果
    }
}

/* 发送温度数据 */
void Send_Temperature(void)
{
    static uint32_t last_send_time = 0;
    uint32_t current_time = 0;
    char temp_buffer[20] = {0};
    
    /* 获取当前精确时间 */
    current_time = GetSysTime_ms();
    
    /* 每1000ms发送一次温度数据 */
    if (current_time - last_send_time >= 1000)
    {
        sprintf(temp_buffer, "Temp: %.1f°C\r\n", current_temp);
        USART_SendString(USART1, temp_buffer);
        last_send_time = current_time;
    }
}

/* 处理串口命令 */
void Process_Serial_Command(void)
{
    char response[30] = {0};
    
    if (serial_rx_data == 0x01)
    {
        /* 返回当前温度阈值 */
        sprintf(response, "Threshold: %.1f°C\r\n", current_threshold);
        USART_SendString(USART1, response);
    }
    else
    {
        /* 无效指令 */
        USART_SendString(USART1, "invalid instruction.\r\n");
    }
}

/* USART接收中断回调函数 */
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        /* 清除中断标志 */
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        
        /* 读取接收到的数据 */
        serial_rx_data = USART_ReceiveData(USART1);
        serial_rx_flag = 1;
    }
}

/* 外部中断处理函数 - 按键中断 */
void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line8) != RESET) // PA8按键中断
    {
        /* 消除按键抖动 - 精确延时 */
        Delay_ms(20);
        
        /* 确认按键按下 */
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0)
        {
            /* 循环切换温度阈值 */
            temp_threshold_index = (temp_threshold_index + 1) % 3;
            current_threshold = temp_thresholds[temp_threshold_index];
            
            /* 通过串口发送新的阈值 */
            char threshold_buffer[30];
            sprintf(threshold_buffer, "New Threshold: %.1f°C\r\n", current_threshold);
            USART_SendString(USART1, threshold_buffer);
        }
        
        /* 清除中断标志位 */
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
}
