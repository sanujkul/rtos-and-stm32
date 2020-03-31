################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Third_Party/FreeRTOS/org/Source/croutine.c \
../Third_Party/FreeRTOS/org/Source/event_groups.c \
../Third_Party/FreeRTOS/org/Source/list.c \
../Third_Party/FreeRTOS/org/Source/queue.c \
../Third_Party/FreeRTOS/org/Source/stream_buffer.c \
../Third_Party/FreeRTOS/org/Source/tasks.c \
../Third_Party/FreeRTOS/org/Source/timers.c 

OBJS += \
./Third_Party/FreeRTOS/org/Source/croutine.o \
./Third_Party/FreeRTOS/org/Source/event_groups.o \
./Third_Party/FreeRTOS/org/Source/list.o \
./Third_Party/FreeRTOS/org/Source/queue.o \
./Third_Party/FreeRTOS/org/Source/stream_buffer.o \
./Third_Party/FreeRTOS/org/Source/tasks.o \
./Third_Party/FreeRTOS/org/Source/timers.o 

C_DEPS += \
./Third_Party/FreeRTOS/org/Source/croutine.d \
./Third_Party/FreeRTOS/org/Source/event_groups.d \
./Third_Party/FreeRTOS/org/Source/list.d \
./Third_Party/FreeRTOS/org/Source/queue.d \
./Third_Party/FreeRTOS/org/Source/stream_buffer.d \
./Third_Party/FreeRTOS/org/Source/tasks.d \
./Third_Party/FreeRTOS/org/Source/timers.d 


# Each subdirectory must supply rules for building sources it contributes
Third_Party/FreeRTOS/org/Source/%.o: ../Third_Party/FreeRTOS/org/Source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F4 -DSTM32F446RETx -DNUCLEO_F446RE -DDEBUG -DSTM32F446xx -DUSE_STDPERIPH_DRIVER -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/freeRTOS_LD2_B1/Config" -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/freeRTOS_LD2_B1/Third_Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F" -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/freeRTOS_LD2_B1/Third_Party/FreeRTOS/org/Source/include" -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/freeRTOS_LD2_B1/StdPeriph_Driver/inc" -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/freeRTOS_LD2_B1/Third_Party/FreeRTOS/org/Source/include" -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/freeRTOS_LD2_B1/inc" -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/freeRTOS_LD2_B1/CMSIS/device" -I"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/freeRTOS_LD2_B1/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


