################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Third_Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/port.c 

OBJS += \
./Third_Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/port.o 

C_DEPS += \
./Third_Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/port.d 


# Each subdirectory must supply rules for building sources it contributes
Third_Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/%.o: ../Third_Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F4 -DSTM32F446RETx -DNUCLEO_F446RE -DDEBUG -DSTM32F446xx -DUSE_STDPERIPH_DRIVER -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/STM32_HelloWOrld/StdPeriph_Driver/inc" -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/STM32_HelloWOrld/Config" -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/STM32_HelloWOrld/Third_Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F" -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/STM32_HelloWOrld/Third_Party/FreeRTOS/org/Source/include" -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/STM32_HelloWOrld/inc" -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/STM32_HelloWOrld/CMSIS/device" -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/STM32_HelloWOrld/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


