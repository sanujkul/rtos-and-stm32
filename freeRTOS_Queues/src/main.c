/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */
//#define  DEBUG_VIA_PRINT

#include <stdio.h>
#include <stdint.h>
#include <string.h>


#include "stm32f4xx.h"
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>			//We will be using software timers
#include <FreeRTOSConfig.h>

#define TRUE 1
#define FALSE 0

#define LED_ON_COMMAND 1
#define LED_OFF_COMMAND 2
#define LED_TOGGLE_COMMAND 3
#define LED_TOGGLE_STOP 4
#define LED_READ_STATUS 5
#define RTC_READ_DATE_TIME 6
#define EXIT_APP 0

char msg[100] = "HelloWorld!";
char usr_msg[250]; 	//FOr testing purpsose only

//Following data structure will store command number and associated arguments
typedef struct APP_CMD{
	uint8_t COMMAND_NUM;
	uint8_t COMMAND_ARGS[10];
}APP_CMD_t;

//task handles
TaskHandle_t cmd_handling_Handle = NULL;
TaskHandle_t cmd_processing_Handle = NULL;
TaskHandle_t menu_print_Handle = NULL;
TaskHandle_t uart_write_Handle = NULL;
//Queue Handles;
xQueueHandle command_queue = NULL;
xQueueHandle uart_write_queue = NULL;
//Software timer handles:
TimerHandle_t led_timer_handle = NULL;

//
uint16_t command_buffer[20];
uint8_t command_len = 0;

//Functions
void cmd_handling_task(void *params);
void cmd_processing_task(void *params);
void menu_print_task(void *params);
void uart_write_task(void *params);

uint16_t getCommandCode(void);
void getArguements(uint8_t *buffer);

static void pvtSetupHardware();
static void pvtSetupUART2();
static void pvtSetupGPIOs();
void printmsg(char *msg);		//Function we'll make to print data over UART

void make_led_on(void);
void make_led_off(void);
void led_toggle_start(uint32_t toggle_duration);
void led_toggle_stop(void);
void read_led_status(char *task_msg);
void read_rtc_info(char *task_msg);
void print_error_msg();
//Software timer callback function
void led_toggle();

//A string send by UART which will be our menu
char menu[] = {"\r\nLED_ON\t --->1, \r\nLED_OFF\t ---> 2, \r\nLED_TOGGLE\t ---> 3, \r\nLED_TOGGLE_OFF\t ---> 4, \r\nLED_READ_STATUS\t --->5, \r\nRTCPRINT_DATETIME\t ---> 6, \r\nEXIT_APP\t ---> 0,\r\nEnter key:"};

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

	sprintf(usr_msg, "This is LED BUTTON starting messege \n");
	printmsg(usr_msg);

	//Creating queues:
	command_queue= xQueueCreate(10, sizeof(APP_CMD_t*));	//40 bytes = size of memory pointer (4 bytes) x 10
	uart_write_queue = xQueueCreate(10, sizeof(char*));

	if((command_queue != NULL) && (uart_write_queue != NULL) ){ //then create 4 tasks
		xTaskCreate(cmd_handling_task,
				"cmd_handling_task",
				500,						//around 2000 bytes ~ 2KB
				NULL,
				2,
				&cmd_handling_Handle); 						//NULL TASK HANDLER

		xTaskCreate(cmd_processing_task,
				"cmd_processing_task",
				500,
				NULL,
				2,
				&cmd_processing_Handle); 						//NULL TASK HANDLER

		xTaskCreate(menu_print_task,
				"menu_print_task",
				500,
				NULL,
				1,
				&menu_print_Handle); 						//NULL TASK HANDLER

		xTaskCreate(uart_write_task,
				"uart_write_task",
				500,
				NULL,
				2,
				&uart_write_Handle); 						//NULL TASK HANDLER
	}else{
		sprintf(usr_msg, "Queue Creation failed\n");
		printmsg(usr_msg);
	}
	vTaskStartScheduler();

	for(;;);
}
/////// /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// RTOS CODES
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void cmd_handling_task(void *params){
	uint16_t command_code = 0;
	APP_CMD_t *new_cmd;
	while(1){
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);

#ifdef DEBUG_VIA_PRINT
		sprintf(usr_msg, "Command Handling Task Woke up \n");
		printmsg(usr_msg);
#endif

		//1. send command to command queue
		new_cmd = (APP_CMD_t*) pvPortMalloc(sizeof(APP_CMD_t));
//To prevent race condition on command_code variable by This task and ISR
//So this function also disables the Interrupts
		taskENTER_CRITICAL();

		command_code = getCommandCode();
		new_cmd->COMMAND_NUM = command_code;
		getArguements(new_cmd->COMMAND_ARGS);

		taskEXIT_CRITICAL();

		//send the command to command wueue
		xQueueSend(command_queue, &new_cmd, portMAX_DELAY);

	}
}

uint16_t getCommandCode(void){
#ifdef DEBUG_VIA_PRINT
		sprintf(usr_msg, "Command is: %d\r\n",(buffer[0]-48));
		printmsg(usr_msg);
#endif
	return (command_buffer[0]-48); //ASCII -> Number
}

void getArguements(uint8_t *buffer){

}


void cmd_processing_task(void *params){
	APP_CMD_t *new_cmd;
	char task_msg[50];

	uint32_t toggle_duration = pdMS_TO_TICKS(500); //Toggle time is 500 ms

	while(1){
		//Reads from the command queue
		xQueueReceive(command_queue,(void*)&new_cmd,portMAX_DELAY);

		if(new_cmd->COMMAND_NUM == LED_ON_COMMAND){
			make_led_on();
		}else if(new_cmd->COMMAND_NUM == LED_OFF_COMMAND){
			make_led_off();
		}else if(new_cmd->COMMAND_NUM == LED_TOGGLE_COMMAND){
			led_toggle_start(toggle_duration);
		}else if(new_cmd->COMMAND_NUM == LED_TOGGLE_STOP){
			led_toggle_stop();
		}else if(new_cmd->COMMAND_NUM == LED_READ_STATUS){
			read_led_status(task_msg);
		}else if(new_cmd->COMMAND_NUM == RTC_READ_DATE_TIME){
			read_rtc_info(task_msg);
		}else{
			print_error_msg();
		}

		vPortFree(new_cmd);
	}
}
//-----------------------------------------------------
void make_led_on(void){
	GPIO_WriteBit(GPIOA,GPIO_Pin_5, Bit_SET);
}

void make_led_off(void){
	GPIO_WriteBit(GPIOA,GPIO_Pin_5, Bit_RESET);
}

void led_toggle_start(uint32_t toggle_duration){

	if(led_timer_handle == NULL){
		//Lets create software timer
		led_timer_handle = xTimerCreate("LED-TIMER",
				toggle_duration,
				pdTRUE,
				NULL,
				led_toggle);

	}
	//2. startt the software timer
	xTimerStart(led_timer_handle,portMAX_DELAY);
}
void led_toggle(){
	GPIO_ToggleBits(GPIOA,GPIO_Pin_5);
}
void led_toggle_stop(void){
	xTimerStop(led_timer_handle,portMAX_DELAY);
}

void read_led_status(char *task_msg){
	sprintf(task_msg,"\r\n LED status is:%d\r\n",GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_5));
	xQueueSend(uart_write_queue,&task_msg,portMAX_DELAY);
}

void read_rtc_info(char *task_msg){
	RTC_TimeTypeDef RTC_time;
	RTC_DateTypeDef RTC_date;

	//read time and date fromRTC peripheral of our microcontroller
	RTC_GetTime(RTC_Format_BIN,&RTC_time);
	RTC_GetDate(RTC_Format_BIN,&RTC_date);

	sprintf(task_msg, "\r\nTime: %02d:02d \rDate: %02d-%02d-%02d \r\n",RTC_time.RTC_Hours,RTC_time.RTC_Minutes,RTC_time.RTC_Seconds,
							RTC_date.RTC_Date, RTC_date.RTC_Month, RTC_date.RTC_Year);
	xQueueSend(uart_write_queue,&task_msg,portMAX_DELAY);
}

void print_error_msg(void){

}
//-----------------------------------------------------
void menu_print_task(void *params){
	char *menuptr = menu;
	while(1){
		//Lets put menu to queue, if full, block until it gets emty
#ifndef DEBUG_VIA_PRINT
		xQueueSend(uart_write_queue,&menuptr, portMAX_DELAY);
#endif
		//lets block here until someone notify
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
	}
}

//This should read the data from the queue and
//send data to UART peripheral
void uart_write_task(void *params){
	char *dataptr = NULL;
	while(1){
		xQueueReceive(uart_write_queue, &dataptr, portMAX_DELAY);
		printmsg(dataptr);
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

//SETTING UP USR LED2 FOR OUTPUT
//SETTING UP BT1 AS INPUT

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

	//Enabling the UART2 byte reception interrupt
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	NVIC_SetPriority(USART2_IRQn,5);
	NVIC_EnableIRQ(USART2_IRQn);


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

/*
 * Read UART data. if receive s, then notify other two task.
 */
void USART2_IRQHandler(void){
	uint16_t data_byte; //to store the recieved data;
	BaseType_t xHigherProrityTaskWoken = pdFALSE;
	//To see what causes the interrupt
	if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE)){
		//a data byte is recieved from UART
		data_byte = USART_ReceiveData(USART2);
#ifdef DEBUG_VIA_PRINT
		sprintf(usr_msg, "Received data is: %d at index %d\r\n",data_byte,command_len);
		printmsg(usr_msg);
#endif
		command_buffer[command_len++] = data_byte; //Masking Interested in only 8 bits
		if(data_byte == 's'){
			command_len = 0;
#ifdef DEBUG_VIA_PRINT
			sprintf(usr_msg, "got s");
			printmsg(usr_msg);
#endif
#ifdef DEBUG_VIA_PRINT
			sprintf(usr_msg, "Received data is: %d at index %d\r\n",command_buffer[0],command_len);
			printmsg(usr_msg);
#endif
//			user finished entering data
			xTaskNotifyFromISR(cmd_handling_Handle,0, eNoAction, &xHigherProrityTaskWoken);
			xTaskNotifyFromISR(menu_print_Handle,0, eNoAction, &xHigherProrityTaskWoken);
		}
	}

//	If above freeRTOS apis wake up any high priroity task
//	then yield the processor to high priority task that
//	just woke up
	if(xHigherProrityTaskWoken){
		taskYIELD();
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
