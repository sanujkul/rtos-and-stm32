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
#include <stdlib.h>


#include "stm32f4xx.h"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <FreeRTOSConfig.h>

#define PRESSED 1
#define NOT_PRESSED 0
			
char msg[100] = "HelloWorld!";
char usr_msg[250]; 	//FOr testing purpsose only

void vManagerTask(void *params);
void vemployeeTask(void *params);

TaskHandle_t xMangerHandle = NULL;
TaskHandle_t xEmployeeHandle = NULL;

xSemaphoreHandle xWork;

xQueueHandle xWorkQueue;

static void pvtSetupUART2();
static void pvtSetupGPIOs();
static void pvtSetupHardware();

void printmsg(char *msg);		//Function we'll make to print data over UART

uint8_t button_status_flag = NOT_PRESSED;

int main(void)
{
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

	sprintf(usr_msg, "This is demo of using semaphore \n");
	printmsg(usr_msg);

	//Before semaphore is used it must be created
	vSemaphoreCreateBinary(xWork);	//To create binry semaphore

	xWorkQueue = xQueueCreate(1,sizeof(unsigned int));

	if((xWork != NULL) && (xWorkQueue != NULL)){

		//Lets create LED Task
		//	STEP3: LETS CREATE 2 Task
		xTaskCreate(vManagerTask,
				"LED-TASK",
				configMINIMAL_STACK_SIZE,
				NULL,
				3,
				&xMangerHandle); 						//NULL TASK HANDLER

		xTaskCreate(vemployeeTask,
				"BUTTON-TASK",
				configMINIMAL_STACK_SIZE,
				NULL,
				1,
				&xEmployeeHandle); 						//NULL TASK HANDLER
	}


	vTaskStartScheduler();

	for(;;);
}
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// RTOS CODES
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void vManagerTask(void *params){
	unsigned int xWorkTicketID;
	portBASE_TYPE xStatus;

	/* Semephore is created in empty mode.
	 * It has to be created, before using it first.
	 */
	xSemaphoreGive(xWork);
	while(1){
//		xWorkTicketID = (rand() & 0x1FF);
		sprintf(usr_msg, "1.TASK 01 manager: Sending Semaphore: \r\n");
		printmsg(usr_msg);

//		xStatus = xQueueSend(xWorkQueue,xWorkTicketID,portMAX_DELAY);
//
//		if(xStatus != pdPASS){
//			sprintf(usr_msg, "1. Queue FULL, could not send \r\n");
//			printmsg(usr_msg);
//		}else{
//			//manager notifying employee by giving semaphore.
//			sprintf(usr_msg, "2. sending semaphore \r\n");
//			printmsg(usr_msg);
//
//			xSemaphoreGive(xWork);
////			After task, just yield, as nothing else to do
//			taskYIELD();
//		}
		xSemaphoreGive(xWork);
		vTaskDelay(100);
	}
}

void vemployeeTask(void *params){
//	unsigned int xWorkTicketID;
//	portBASE_TYPE xStatus;

	while(1){
		sprintf(usr_msg, "3.TASK 02 employee: Waiting for semaphore \r\n");
		printmsg(usr_msg);

		//Task will never block if semaphore is ont available
		xSemaphoreTake(xWork,portMAX_DELAY);

		sprintf(usr_msg, "4.TASK 02 employee: GOT for semaphore \r\n");
		printmsg(usr_msg);

//		xStatus = xQueueReceive(xWorkQueue,&xWorkTicketID,0);
//
//		sprintf(usr_msg, "5.got data form queue \r\n");
//		printmsg(usr_msg);
//
//		if(xStatus == pdPASS){
//			//Employee do work
//			sprintf(usr_msg, "Employee doing random work: %d\r\n",xWorkTicketID);
//			printmsg(usr_msg);
//		}else{
//			sprintf(usr_msg, "Queue EMPTY, employee could not read \r\n");
//			printmsg(usr_msg);
//		}
	}
}
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// HARDWARE SETUP
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

static void pvtSetupHardware(){
	//Setting up UART2
	pvtSetupUART2();
	pvtSetupGPIOs();

}

static void pvtSetupGPIOs(){
	//Initializing clocks for this GPIO
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE);

	//Setting up LED and button
	GPIO_InitTypeDef led_gpio;
	memset(&led_gpio, 0,sizeof(led_gpio));
	//	2. PA2 is UART2_Tx, PA3 is Rx in UART2 when in Alternative mode 7
	led_gpio.GPIO_Pin = GPIO_Pin_5;
	led_gpio.GPIO_Mode = GPIO_Mode_OUT;
	//Using Push pull, if we have chposen open drain then we have to use resistor for HIGH-LOW
	led_gpio.GPIO_OType = GPIO_OType_PP;
	led_gpio.GPIO_Speed = GPIO_Low_Speed;
	led_gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &led_gpio);

	//2. FOR Button:
	GPIO_InitTypeDef button_gpio;
	memset(&button_gpio, 0,sizeof(button_gpio));
	//	2. PA2 is UART2_Tx, PA3 is Rx in UART2 when in Alternative mode 7
	button_gpio.GPIO_Pin = GPIO_Pin_13;
	button_gpio.GPIO_Mode = GPIO_Mode_IN;
	//Using Push pull, if we have chposen open drain then we have to use resistor for HIGH-LOW
	button_gpio.GPIO_OType = GPIO_OType_PP;
	button_gpio.GPIO_Speed = GPIO_Low_Speed;
	button_gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &button_gpio);




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

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
