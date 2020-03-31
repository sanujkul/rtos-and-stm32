/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>


#include "stm32f4xx.h"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <FreeRTOSConfig.h>

//If you want to use semihosting (printf) uncomment following line.
//#define SEMI_HOSTING

TaskHandle_t xTaskHandle1 = NULL;
TaskHandle_t xTaskHandle2 = NULL;

xSemaphoreHandle UART_mutualEx = NULL;

char msg[100] = "HelloWorld!";
char usr_msg[250]; 	//FOr testing purpsose only

void vTask1_handler(void *params);
void vTask2_handler(void *params);

static void pvtSetupUART2();
static void pvtSetupGPIOs();
static void pvtSetupHardware();

void printmsg(char *msg);		//Function we'll make to print data over UART

void lockCritical_UART();
void releaseCritical_UART();

//use for semihosting
#ifdef SEMI_HOSTING
extern void initialise_monitor_handles();
#endif


int main(void)
{
#ifdef SEMI_HOSTING
	initialise_monitor_handles();
	printf("This is Helo world example code \n");
#endif
//	DWT->CTRL |= (1 << 0);  //If we are using Segger system view
//	STEP1: CLOCK:
//	By default, system clock would be SystemCoreClock = 180 MHz (in system_stm...c file)
//	By RCC_DeInit() : HSI ON and used as system clock source
//	                - HSE, PLL and PLLI2S OFF
//					- Therefore system clock = 16 MHz (internal RC oscillator)
	RCC_DeInit();
//	STEP2: Update the SystemCoreClock variable to new one
	SystemCoreClockUpdate(); //This function will update the SystemCoreClock variable to new one
	//Setting up USART2 to send data
	pvtSetupHardware();

	sprintf(usr_msg, "This is hello World starting messege \n");
	printmsg(usr_msg);

//	STEP3: LETS CREATE 2 Task
	xTaskCreate(vTask1_handler,
			"task1",
			configMINIMAL_STACK_SIZE,
			NULL,
			2,
			&xTaskHandle1);

	xTaskCreate(vTask2_handler,
			"task2",
			configMINIMAL_STACK_SIZE,
			NULL,
			2,
			&xTaskHandle2);

	//Making a binary Semaphore
	UART_mutualEx = xSemaphoreCreateBinary();
	xSemaphoreGive(UART_mutualEx);			//For starting giving a sema

//	//Start the scheduler
	vTaskStartScheduler();

//	This part will never be reached
	for(;;);
}

void vTask1_handler(void *params){
	while(1){
#ifdef SEMI_HOSTING
		printf("Hello World: Task 1\n");
#endif
		lockCritical_UART();
		sprintf(usr_msg, "Hello World: Task 1\n");
		printmsg(usr_msg);
		releaseCritical_UART();
		vTaskDelay(2);
	}
}

void vTask2_handler(void *params){
	while(1){
#ifdef SEMI_HOSTING
		printf("Hello World: Task 2\n");
#endif
		lockCritical_UART();
		sprintf(usr_msg, "Hello World: Task 2\n");
		printmsg(usr_msg);
		releaseCritical_UART();

		vTaskDelay(2);
	}
}

void lockCritical_UART(){
	xSemaphoreTake(UART_mutualEx,portMAX_DELAY);
}
void releaseCritical_UART(){
	xSemaphoreGive(UART_mutualEx);
}
///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

static void pvtSetupHardware(){
	//Setting up UART2
	pvtSetupUART2();
	pvtSetupGPIOs();
}

static void pvtSetupGPIOs(){
	//Setting up LED and button
	GPIO_InitTypeDef gpio_uart_pins;
	memset(&gpio_uart_pins, 0,sizeof(gpio_uart_pins));
	//	2. PA2 is UART2_Tx, PA3 is Rx in UART2 when in Alternative mode 7
	gpio_uart_pins.GPIO_Pin = GPIO_Pin_5;
	gpio_uart_pins.GPIO_Mode = GPIO_Mode_OUT;
	//Using Push pull, if we have chposen open drain then we have to use resistor for HIGH-LOW
	gpio_uart_pins.GPIO_OType = GPIO_OType_PP;



}

static void pvtSetupUART2(){
	GPIO_InitTypeDef gpio_uart_pins;
	USART_InitTypeDef uart2_init;
	//Clearing all bits of these local variables:
	memset(&gpio_uart_pins, 0,sizeof(gpio_uart_pins));
	memset(&uart2_init, 0,sizeof(uart2_init));

	//	1. Enable the UART and GPIOA peripheral clock
	//Uart2 is connected to AHPB1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
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
	uart2_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	uart2_init.USART_Parity = USART_Parity_No;
	uart2_init.USART_StopBits = USART_StopBits_1;
	uart2_init.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &uart2_init);

//	5. Enable the USART2 peripheral
	USART_Cmd(USART2, ENABLE);
}

void printmsg(char *msg){
	uint32_t i = 0;
	for(i=0; i<strlen(msg); i++){
		//When Tx register (TXE) is empty, then send, else wait
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) != SET);
//		int j = 0;
//		for(j=0; j<10000000;j++){};
		USART_SendData(USART2, msg[i]);
	}
}
