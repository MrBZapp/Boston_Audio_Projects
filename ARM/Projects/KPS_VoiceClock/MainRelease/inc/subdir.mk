################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../inc/GlobalDEF.c \
../inc/KPS_Exciter.c \
../inc/LocalMIDIFunc.c 

OBJS += \
./inc/GlobalDEF.o \
./inc/KPS_Exciter.o \
./inc/LocalMIDIFunc.o 

C_DEPS += \
./inc/GlobalDEF.d \
./inc/KPS_Exciter.d \
./inc/LocalMIDIFunc.d 


# Each subdirectory must supply rules for building sources it contributes
inc/%.o: ../inc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DDEBUG -D__CODE_RED -DCORE_M0PLUS -D__MTB_BUFFER_SIZE=256 -D__USE_LPCOPEN -DNO_BOARD_LIB -D__LPC8XX__ -I"C:\Users\Matt\Boston_Audio_Projects\Boston_Audio_Projects\ARM\Projects\KPS_VoiceClock\inc" -I"C:\Users\Matt\Documents\LPCXpresso_7.3.0_186\workspace\lpc_chip_8xx_lib\inc" -I"C:\Users\Matt\Boston_Audio_Projects\Boston_Audio_Projects\ARM\BAP_LPC8xx_lib\inc" -Os -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


