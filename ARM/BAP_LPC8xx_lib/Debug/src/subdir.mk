################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BAP_32Sel.c \
../src/BAP_Clk.c \
../src/BAP_Envelope.c \
../src/BAP_I2C.c \
../src/BAP_MAX1164x_ADC.c \
../src/BAP_Midi.c \
../src/BAP_SPIMemory.c \
../src/BAP_Scheduler.c \
../src/BAP_TLC_DAC.c \
../src/BAP_TLV_DAC.c \
../src/BAP_WaveGen.c \
../src/BAP_math.c \
../src/FrequencyMaps.c 

OBJS += \
./src/BAP_32Sel.o \
./src/BAP_Clk.o \
./src/BAP_Envelope.o \
./src/BAP_I2C.o \
./src/BAP_MAX1164x_ADC.o \
./src/BAP_Midi.o \
./src/BAP_SPIMemory.o \
./src/BAP_Scheduler.o \
./src/BAP_TLC_DAC.o \
./src/BAP_TLV_DAC.o \
./src/BAP_WaveGen.o \
./src/BAP_math.o \
./src/FrequencyMaps.o 

C_DEPS += \
./src/BAP_32Sel.d \
./src/BAP_Clk.d \
./src/BAP_Envelope.d \
./src/BAP_I2C.d \
./src/BAP_MAX1164x_ADC.d \
./src/BAP_Midi.d \
./src/BAP_SPIMemory.d \
./src/BAP_Scheduler.d \
./src/BAP_TLC_DAC.d \
./src/BAP_TLV_DAC.d \
./src/BAP_WaveGen.d \
./src/BAP_math.d \
./src/FrequencyMaps.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DDEBUG -D__CODE_RED -DCORE_M0PLUS -D__USE_LPCOPEN -D__LPC8XX__ -I"C:\Users\Matt\Boston_Audio_Projects\Boston_Audio_Projects\ARM\BAP_LPC8xx_lib\inc" -I"C:\Users\Matt\Documents\LPCXpresso_7.3.0_186\workspace\lpc_chip_8xx_lib\inc" -Os -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


