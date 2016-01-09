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
all: Metaphase-MASK.axf

# Tool invocations
Metaphase-MASK.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\Matt\Boston_Audio_Projects\Boston_Audio_Projects\ARM\BAP_LPC8xx_lib\Debug" -L"C:\Users\Matt\Documents\LPCXpresso_7.3.0_186\workspace\lpc_chip_8xx_lib\Debug" -Xlinker -Map="Metaphase-MASK.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "Metaphase-MASK_Debug.ld" -o "Metaphase-MASK.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) Metaphase-MASK.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "Metaphase-MASK.axf" ; arm-none-eabi-objdump -S "Metaphase-MASK.axf" > "Metaphase-MASK.txt" ; arm-none-eabi-objcopy -O binary "Metaphase-MASK.axf" "Metaphase-MASK.bin" ; arm-none-eabi-objcopy -O ihex "Metaphase-MASK.axf" "Metaphase-MASK.hex" ; checksum -p LPC812 -d "Metaphase-MASK.bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
