################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32.s 

OBJS += \
./startup/startup_stm32.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/STM32_UART/StdPeriph_Driver/inc" -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/STM32_UART/inc" -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/STM32_UART/CMSIS/device" -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/STM32_UART/CMSIS/core" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


