################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BAP_WaveGen.c \
../src/KPS_VoiceClock.c \
../src/cr_startup_lpc8xx.c \
../src/crp.c \
../src/mtb.c \
../src/sysinit.c 

OBJS += \
./src/BAP_WaveGen.o \
./src/KPS_VoiceClock.o \
./src/cr_startup_lpc8xx.o \
./src/crp.o \
./src/mtb.o \
./src/sysinit.o 

C_DEPS += \
./src/BAP_WaveGen.d \
./src/KPS_VoiceClock.d \
./src/cr_startup_lpc8xx.d \
./src/crp.d \
./src/mtb.d \
./src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DNDEBUG -D__CODE_RED -DCORE_M0PLUS -D__MTB_BUFFER_SIZE=256 -D__USE_LPCOPEN -DNO_BOARD_LIB -D__LPC8XX__ -I"C:\Users\Matt\Boston_Audio_Projects\Boston_Audio_Projects\ARM\Projects\KPS_VoiceClock\inc" -I"C:\Users\Matt\Documents\LPCXpresso_7.3.0_186\workspace\lpc_chip_8xx_lib\inc" -Os -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


