################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: SimpleMidiModule.axf

# Tool invocations
SimpleMidiModule.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\Matt\Boston_Audio_Projects\Boston_Audio_Projects\ARM\BAP_LPC8xx_lib\Debug" -L"C:\Users\Matt\Documents\LPCXpresso_7.3.0_186\workspace\lpc_chip_8xx\Debug" -Xlinker -Map="SimpleMidiModule.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "SimpleMidiModule_LPC810_Config.ld" -o "SimpleMidiModule.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) SimpleMidiModule.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "SimpleMidiModule.axf" ; arm-none-eabi-objdump -S "SimpleMidiModule.axf" > "SimpleMidiModule.txt" ; arm-none-eabi-objcopy -O binary "SimpleMidiModule.axf" "SimpleMidiModule.bin" ; arm-none-eabi-objcopy -O ihex "SimpleMidiModule.axf" "SimpleMidiModule.hex" ; checksum -p LPC810 -d "SimpleMidiModule.bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
