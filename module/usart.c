/* 
 * 文件名: usart.c
 * 描述: 串口通信模块
 * 功能: 配置串口通信，实现数据发送和接收功能
 */

#include "stm32f10x.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief  配置USART模块
 * @param  无
 * @retval 无
 */
void USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* 使能USART1和GPIOA时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    /* 配置USART1 Tx (PA9) 为复用推挽输出 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 配置USART1 Rx (PA10) 为浮空输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 配置USART1参数 */
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    
    /* 配置USART1中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* 使能USART1接收中断 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    /* 使能USART1 */
    USART_Cmd(USART1, ENABLE);
}

/**
 * @brief  发送一个字节数据
 * @param  USARTx: 指定的USART端口
 * @param  data: 要发送的数据
 * @retval 无
 */
void USART_SendByte(USART_TypeDef* USARTx, uint8_t data)
{
    /* 等待发送缓冲区为空 */
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    
    /* 发送数据 */
    USART_SendData(USARTx, data);
    
    /* 等待发送完成 */
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
}

/**
 * @brief  发送字符串
 * @param  USARTx: 指定的USART端口
 * @param  str: 要发送的字符串
 * @retval 无
 */
void USART_SendString(USART_TypeDef* USARTx, char* str)
{
    uint16_t i = 0;
    
    /* 循环发送每个字符 */
    while(str[i] != '\0')
    {
        USART_SendByte(USARTx, str[i++]);
    }
}

/**
 * @brief  接收一个字节数据
 * @param  USARTx: 指定的USART端口
 * @retval 接收到的数据
 */
uint8_t USART_ReceiveByte(USART_TypeDef* USARTx)
{
    /* 等待接收缓冲区非空 */
    while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
    
    /* 返回接收到的数据 */
    return (uint8_t)USART_ReceiveData(USARTx);
}

/* 重定向printf函数 */
int fputc(int ch, FILE *f)
{
    /* 发送一个字节数据到USART1 */
    USART_SendData(USART1, (uint8_t)ch);
    
    /* 等待发送完毕 */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    
    return ch;
}
