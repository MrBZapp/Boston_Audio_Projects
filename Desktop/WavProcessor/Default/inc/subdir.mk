################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../inc/delayLine.c \
../inc/lfo.c \
../inc/wavFile.c \
../inc/wavProcesses.c \
../inc/wavSample.c 

OBJS += \
./inc/delayLine.o \
./inc/lfo.o \
./inc/wavFile.o \
./inc/wavProcesses.o \
./inc/wavSample.o 

C_DEPS += \
./inc/delayLine.d \
./inc/lfo.d \
./inc/wavFile.d \
./inc/wavProcesses.d \
./inc/wavSample.d 


# Each subdirectory must supply rules for building sources it contributes
inc/%.o: ../inc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


