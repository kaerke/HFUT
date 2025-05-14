/* 
 * 文件名: usart.h
 * 描述: 串口通信模块头文件
 * 功能: 声明串口通信相关函数
 */

#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

/* 函数声明 */
void USART_Config(void);                                // 配置USART
void USART_SendByte(USART_TypeDef* USARTx, uint8_t data); // 发送一个字节数据
void USART_SendString(USART_TypeDef* USARTx, char* str);  // 发送字符串
uint8_t USART_ReceiveByte(USART_TypeDef* USARTx);        // 接收一个字节数据

#endif /* __USART_H */
