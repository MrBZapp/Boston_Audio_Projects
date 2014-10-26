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
all: KPS_VoiceClock.axf

# Tool invocations
KPS_VoiceClock.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\Matt\Documents\LPCXpresso_7.3.0_186\workspace\lpc_chip_8xx_lib\Debug" -Xlinker -Map="KPS_VoiceClock.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "KPS_VoiceClock_Debug.ld" -o "KPS_VoiceClock.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) KPS_VoiceClock.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "KPS_VoiceClock.axf" ; arm-none-eabi-objcopy -O binary "KPS_VoiceClock.axf" "KPS_VoiceClock.bin" ; arm-none-eabi-objcopy -O ihex "KPS_VoiceClock.axf" "KPS_VoiceClock.hex" ; checksum -p LPC810 -d "KPS_VoiceClock.bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
