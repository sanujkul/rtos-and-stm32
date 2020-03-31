/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include "stm32f4xx.h"


static void prvSetupHardware(void);
static void prvSetupUart(void);
void printmsg(char *msg);

//some macros
//#define TRUE 1
//#define FALSE 0
//#define AVAILABLE TRUE
//#define NOT_AVAILABLE FALSE

//Global variable section
char usr_msg[250]={0};
//uint8_t UART_ACCESS_KEY = AVAILABLE;

int main(void)
{
	DWT->CTRL |= (1 << 0);//Enable CYCCNT in DWT_CTRL.
	//1.  Reset the RCC clock configuration to the default reset state.
		//HSI ON, PLL OFF, HSE OFF, system clock = 16MHz, cpu_clock = 16MHz
		RCC_DeInit();

		//2. update the SystemCoreClock variable
		SystemCoreClockUpdate();

		prvSetupHardware();

		sprintf(usr_msg,"This is hello-world application starting\r\n");
		printmsg(usr_msg);

		for(;;);
}


static void pvtSetupUART2(){
	GPIO_InitTypeDef gpio_uart_pins;
	USART_InitTypeDef uart2_init;
	//Clearing all bits of these local variables:
	memset(&gpio_uart_pins, 0,sizeof(gpio_uart_pins));
	memset(&uart2_init, 0,sizeof(uart2_init));

	//	1. Enable the UART and GPIOA peripheral clock
	//Uart2 is connected to AHPB1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);								//Mistake 1 spotted:  RCCAHB1 kar rakha tha
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);

	//	2. PA2 is UART2_Tx, PA3 is Rx in UART2 when in Alternative mode 7
	gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
	gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &gpio_uart_pins);

	//	3. Alternating Function (AF) Mode Setting for the pins
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //PA2 AF set to UART2_Tx
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); //PA3 AF set to UART2_Rx

	//	4. USART PARAMETER Initialisation
	uart2_init.USART_BaudRate = 115200;
	uart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart2_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;									//Mistake 2 spotted:  USART_Mode_Tx | USART_Mode_Tx
	uart2_init.USART_Parity = USART_Parity_No;
	uart2_init.USART_StopBits = USART_StopBits_1;
	uart2_init.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &uart2_init);

//	5. Enable the USART2 peripheral
	USART_Cmd(USART2, ENABLE);
}

//static void prvSetupUart(void)
//{
//	GPIO_InitTypeDef gpio_uart_pins;
//	USART_InitTypeDef uart2_init;
//
//	//1. Enable the UART2  and GPIOA Peripheral clock
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
//
//	//PA2 is UART2_TX, PA3 is UART2_RX
//
//	//2. Alternate function configuration of MCU pins to behave as UART2 TX and RX
//
//	//zeroing each and every member element of the structure
//	memset(&gpio_uart_pins,0,sizeof(gpio_uart_pins));
//
//	gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
//	gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
//	gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
//	gpio_uart_pins.GPIO_OType= GPIO_OType_PP;
//	gpio_uart_pins.GPIO_Speed = GPIO_High_Speed;
//	GPIO_Init(GPIOA, &gpio_uart_pins);
//
//
//	//3. AF mode settings for the pins
//    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //PA2
//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //PA3
//
//	//4. UART parameter initializations
//	//zeroing each and every member element of the structure
//	memset(&uart2_init,0,sizeof(uart2_init));
//
//	uart2_init.USART_BaudRate = 115200;
//	uart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	uart2_init.USART_Mode =  USART_Mode_Tx | USART_Mode_Rx;
//	uart2_init.USART_Parity = USART_Parity_No;
//	uart2_init.USART_StopBits = USART_StopBits_1;
//	uart2_init.USART_WordLength = USART_WordLength_8b;
//	USART_Init(USART2,&uart2_init);
//
//
//	//5. Enable the UART2 peripheral
//	USART_Cmd(USART2,ENABLE);
//
//}

static void prvSetupHardware(void)
{
	//setup UART2
	pvtSetupUART2();
}

//void printmsg(char *msg){
//	uint32_t i = 0;
//	for(i=0; i<sizeof(msg); i++){
//		//When Tx register (TXE) is empty, then send, else wait
//		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) != SET);
//		USART_SendData(USART2, msg[i]);
//	}
//}
void printmsg(char *msg)
{
	for(uint32_t i=0; i < strlen(msg); i++)
	{
		while ( USART_GetFlagStatus(USART2,USART_FLAG_TXE) != SET);
		USART_SendData(USART2,msg[i]);
	}

	while ( USART_GetFlagStatus(USART2,USART_FLAG_TC) != SET);

}
