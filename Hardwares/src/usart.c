#include <stdio.h>
#include "air105.h"
#include "usart.h"

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
	 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/// <summary>	----------USART specific based on MH1903S---------- </summary>

#define DEBUG_USART_BAUDRATE						115200

#define DEBUG_USART									UART0

#define DEBUG_USART_CLK_ENABLE						SYSCTRL_APBPeriph_UART0

#define DEBUG_USART_TX_PORT							GPIOA
#define DEBUG_USART_TX_Pin							GPIO_Pin_1
#define DEBUG_USART_TX_Remap						GPIO_Remap_0

#define DEBUG_USART_RX_PORT							GPIOA
#define DEBUG_USART_RX_Pin							GPIO_Pin_0
#define DEBUG_USART_RX_Remap						GPIO_Remap_0

#define DEBUG_USART_IRQHandler						/*USART1_IRQHandler*/

/// <summary>	----------USART specific based on MH1903S---------- </summary>

uint8_t USART_RX_BUF[200];

uint16_t USART_RX_STA = 0;

void USART_Init(void)
{
	UART_InitTypeDef UART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/// <summary>	PIN	</summary>
	SYSCTRL_APBPeriphClockCmd(DEBUG_USART_CLK_ENABLE | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(DEBUG_USART_CLK_ENABLE, ENABLE);

	GPIO_PinRemapConfig(DEBUG_USART_TX_PORT, DEBUG_USART_TX_Pin, DEBUG_USART_TX_Remap);

	GPIO_PinRemapConfig(DEBUG_USART_RX_PORT, DEBUG_USART_RX_Pin, DEBUG_USART_RX_Remap);

	UART_InitStructure.UART_BaudRate = DEBUG_USART_BAUDRATE;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;

	UART_Init(DEBUG_USART, &UART_InitStructure);

	/// <summary>	IT Config	</summary>
	UART_ITConfig(UART0, UART_IT_RX_RECVD, ENABLE);

	/// <summary>	NVIC	</summary>
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);

	NVIC_InitStructure.NVIC_IRQChannel = UART0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}

//int fputc(int ch, FILE* f)
//{
//	while (!UART_IsTXEmpty(DEBUG_USART));

//	UART_SendData(DEBUG_USART, (uint8_t)ch);

//	return ch;
//}
