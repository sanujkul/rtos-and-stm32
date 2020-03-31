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
#include <FreeRTOSConfig.h>

#define PRESSED 1
#define NOT_PRESSED 0
			
char msg[100] = "HelloWorld!";
char usr_msg[250]; 	//FOr testing purpsose only

void led_task_handler(void *params);
void button_task_handler(void *params);

static void pvtSetupUART2();
static void pvtSetupGPIOs();
static void pvtSetupHardware();
void rtos_delay(uint32_t delayTime);

void printmsg(char *msg);		//Function we'll make to print data over UART

uint8_t button_status_flag = NOT_PRESSED;
TaskHandle_t xLEDHandle = NULL;
TaskHandle_t xBUTTONHandle = NULL;

int main(void)
{
	RCC->AHB1ENR
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

	sprintf(usr_msg, "This is Notification for LED from BUTTON starting messege \n");
	printmsg(usr_msg);

	//Lets create LED Task
	//	STEP3: LETS CREATE 2 Task
	xTaskCreate(led_task_handler,
			"LED-TASK",
			500,						//Stack size 500 words = 2000 bytes
			NULL,
			2,
			&xLEDHandle); 						//NULL TASK HANDLER

	xTaskCreate(button_task_handler,
			"BUTTON-TASK",
			500,
			NULL,
			2,
			&xBUTTONHandle); 						//NULL TASK HANDLER

	vTaskStartScheduler();

	for(;;);
}
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// RTOS CODES
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void led_task_handler(void *params){
	uint32_t curr_notification_Value = 0;
	while(1){
		if(xTaskNotifyWait(0,0,&curr_notification_Value,portMAX_DELAY) == pdTRUE){
			GPIO_ToggleBits(GPIOA,GPIO_Pin_5);
			sprintf(usr_msg, "Notification is received: Count = %ld \r\n",curr_notification_Value);
			printmsg(usr_msg);
		}
	}

}

void button_task_handler(void *params){
	while(1){
		//Read the button and update button_status_flag
		uint8_t val = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);

		if(val != (uint8_t)Bit_SET){
			//To compensate for switch debouncing
			rtos_delay(200);
			sprintf(usr_msg, "Button BIT not SET,  pressed \r \n");
			printmsg(usr_msg);

			xTaskNotify(xLEDHandle,0x00,eIncrement);

		}


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
//delayTime is in milliseconds
void rtos_delay(uint32_t delayTime){
	uint32_t currentTickCount = xTaskGetTickCount();
	uint32_t delay_in_ticks = ((delayTime/1000)*configTICK_RATE_HZ);
//	Hang here for delayTime amount of Tick counts
	while(xTaskGetTickCount() - currentTickCount < delay_in_ticks);
}
