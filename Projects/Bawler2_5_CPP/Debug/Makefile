################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../_baplib


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../Bawler2_5_CPP.cpp \
../_baplib/Button.cpp \
../_baplib/FlagHandler.cpp \
../_baplib/MidiDevice.cpp \
../_baplib/PeripheralHandler.cpp \
../_baplib/Scheduler.cpp \
../_baplib/SoftTimer.cpp \
../_baplib/Uint8_Buffer.cpp \
../_baplib/FreqGen.cpp \
../_baplib/HardwareTimer.cpp \
../_baplib/TWI.cpp \
../_baplib/USART.cpp


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
Bawler2_5_CPP.o \
_baplib/Button.o \
_baplib/FlagHandler.o \
_baplib/MidiDevice.o \
_baplib/PeripheralHandler.o \
_baplib/Scheduler.o \
_baplib/SoftTimer.o \
_baplib/Uint8_Buffer.o \
_baplib/FreqGen.o \
_baplib/HardwareTimer.o \
_baplib/TWI.o \
_baplib/USART.o

OBJS_AS_ARGS +=  \
Bawler2_5_CPP.o \
_baplib/Button.o \
_baplib/FlagHandler.o \
_baplib/MidiDevice.o \
_baplib/PeripheralHandler.o \
_baplib/Scheduler.o \
_baplib/SoftTimer.o \
_baplib/Uint8_Buffer.o \
_baplib/FreqGen.o \
_baplib/HardwareTimer.o \
_baplib/TWI.o \
_baplib/USART.o

C_DEPS +=  \
Bawler2_5_CPP.d \
_baplib/Button.d \
_baplib/FlagHandler.d \
_baplib/MidiDevice.d \
_baplib/PeripheralHandler.d \
_baplib/Scheduler.d \
_baplib/SoftTimer.d \
_baplib/Uint8_Buffer.d \
_baplib/FreqGen.d \
_baplib/HardwareTimer.d \
_baplib/TWI.d \
_baplib/USART.d

C_DEPS_AS_ARGS +=  \
Bawler2_5_CPP.d \
_baplib/Button.d \
_baplib/FlagHandler.d \
_baplib/MidiDevice.d \
_baplib/PeripheralHandler.d \
_baplib/Scheduler.d \
_baplib/SoftTimer.d \
_baplib/Uint8_Buffer.d \
_baplib/FreqGen.d \
_baplib/HardwareTimer.d \
_baplib/TWI.d \
_baplib/USART.d

OUTPUT_FILE_PATH +=Bawler2_5_CPP.elf

OUTPUT_FILE_PATH_AS_ARGS +=Bawler2_5_CPP.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

# AVR32/GNU C Compiler

























./%.o: .././%.cpp
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DDEBUG  -I"../../../BAPlib" -I"../../../avrlib" -I"../../../avr-stl/include"  -O3 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g3 -Wundef -mmcu=attiny4313 -c -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

_baplib/%.o: ../_baplib/%.cpp
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DDEBUG  -I"../../../BAPlib" -I"../../../avrlib" -I"../../../avr-stl/include"  -O3 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g3 -Wundef -mmcu=attiny4313 -c -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP)
	@echo Building target: $@
	@echo Invoking: AVR8/GNU Linker : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="Bawler2_5_CPP.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=attiny4313  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "Bawler2_5_CPP.elf" "Bawler2_5_CPP.hex"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "Bawler2_5_CPP.elf" "Bawler2_5_CPP.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "Bawler2_5_CPP.elf" > "Bawler2_5_CPP.lss"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "Bawler2_5_CPP.elf" "Bawler2_5_CPP.srec"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-size.exe" "Bawler2_5_CPP.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "Bawler2_5_CPP.elf" "Bawler2_5_CPP.a" "Bawler2_5_CPP.hex" "Bawler2_5_CPP.lss" "Bawler2_5_CPP.eep" "Bawler2_5_CPP.map" "Bawler2_5_CPP.srec" "Bawler2_5_CPP.usersignatures"
	