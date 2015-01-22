################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../inc/jsmn/jsmn.c 

OBJS += \
./inc/jsmn/jsmn.o 

C_DEPS += \
./inc/jsmn/jsmn.d 


# Each subdirectory must supply rules for building sources it contributes
inc/jsmn/%.o: ../inc/jsmn/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


