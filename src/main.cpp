/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include "Led.h"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"



int main(int argc, char* argv[]) {
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef usart;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	//Tx
	gpio.GPIO_Pin = GPIO_Pin_9;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio);

	//Rx
	gpio.GPIO_Pin = GPIO_Pin_10;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpio);

	usart.USART_BaudRate = 9600;
	usart.USART_WordLength = USART_WordLength_8b;
	usart.USART_StopBits = USART_StopBits_1;
	usart.USART_Parity = USART_Parity_No;
	usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &usart);
	USART_Cmd(USART1, ENABLE);

	int i;
	char rxbuf[64];
	int rxbufPos = 0;

	Led leds[] = {Led(GPIOA, GPIO_Pin_0),
				  Led(GPIOA, GPIO_Pin_1),
				  Led(GPIOA, GPIO_Pin_2),
				  Led(GPIOA, GPIO_Pin_3),
				  Led(GPIOB, GPIO_Pin_14),
				  Led(GPIOB, GPIO_Pin_13),
				  Led(GPIOB, GPIO_Pin_15),
				  Led(GPIOB, GPIO_Pin_12),
				  Led(GPIOB, GPIO_Pin_7),
				  Led(GPIOB, GPIO_Pin_6)};

	while (1) {

		while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		rxbuf[rxbufPos++] = USART_ReceiveData(USART1);

		if ((rxbuf[rxbufPos-1] == '\n' || rxbuf[rxbufPos-1] == '\r')
			  && rxbufPos != 0) {

			for (i = 0; i < rxbufPos; i++) {
				USART_SendData(USART1, rxbuf[i]);
				while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
			}
			rxbufPos = 0;

			int percents = 100;
			sscanf(rxbuf, "%d", &percents);

			int ledsCount = percents / 10;

			for (i = 0; i < ledsCount; i++)
				if (i > 1)
					leds[i].on();
				else
					leds[i].off();

			for (i = ledsCount; i < 10; i++)
				if (i > 1)
					leds[i].off();
				else
					leds[i].on();
		}
    }
}

#pragma GCC diagnostic pop


// ----------------------------------------------------------------------------
