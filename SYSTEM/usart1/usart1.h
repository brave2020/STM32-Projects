
#ifndef __USART1_H
#define __USART1_H

#include "stdio.h"     
#include "stdarg.h"			
#include "string.h"
#include "stm32f10x.h"  

#define USART_DEBUG		USART1		//���Դ�ӡ��ʹ�õĴ�����
#define USART1_TXBUFF_SIZE   256   		//���崮��1���ͻ�������СΪ256�ֽ�

void Usart1_Init(unsigned int);     	//����1 ��ʼ������
void u1_printf(char*, ...) ;         	//����1 printf����
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);

#endif


