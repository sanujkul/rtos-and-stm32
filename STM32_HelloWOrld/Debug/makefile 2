################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include startup/subdir.mk
-include src/subdir.mk
-include Third_Party/FreeRTOS/org/Source/portable/MemMang/subdir.mk
-include Third_Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/subdir.mk
-include Third_Party/FreeRTOS/org/Source/subdir.mk
-include StdPeriph_Driver/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: STM32_HelloWOrld.elf

# Tool invocations
STM32_HelloWOrld.elf: $(OBJS) $(USER_OBJS) /Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/STM32_HelloWOrld/LinkerScript.ld
	@echo 'Building target: $@'
	@echo 'Invoking: MCU GCC Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -specs=rdimon.specs -lc -lrdimon -T"/Users/sanujkul/Documents/Udemy/STM-RTOS-Workspace/STM32_HelloWOrld/LinkerScript.ld" -Wl,-Map=output.map -Wl,--gc-sections -o "STM32_HelloWOrld.elf" @"objects.list" $(USER_OBJS) $(LIBS) -lm
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

post-build:
	-@echo 'Generating binary and Printing size information:'
	arm-none-eabi-objcopy -O binary "STM32_HelloWOrld.elf" "STM32_HelloWOrld.bin"
	arm-none-eabi-size "STM32_HelloWOrld.elf"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
