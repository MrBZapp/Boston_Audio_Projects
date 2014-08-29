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
../_BAPlib \
../_AVRLib


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../KPSSynth.c \
../_AVRLib/buffer.c \
../_AVRLib/timer.c \
../_AVRLib/uart.c \
../_BAPlib/MidiDevice.c \
../_BAPlib/WGM.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
KPSSynth.o \
_AVRLib/buffer.o \
_AVRLib/timer.o \
_AVRLib/uart.o \
_BAPlib/MidiDevice.o \
_BAPlib/WGM.o

OBJS_AS_ARGS +=  \
KPSSynth.o \
_AVRLib/buffer.o \
_AVRLib/timer.o \
_AVRLib/uart.o \
_BAPlib/MidiDevice.o \
_BAPlib/WGM.o

C_DEPS +=  \
KPSSynth.d \
_AVRLib/buffer.d \
_AVRLib/timer.d \
_AVRLib/uart.d \
_BAPlib/MidiDevice.d \
_BAPlib/WGM.d

C_DEPS_AS_ARGS +=  \
KPSSynth.d \
_AVRLib/buffer.d \
_AVRLib/timer.d \
_AVRLib/uart.d \
_BAPlib/MidiDevice.d \
_BAPlib/WGM.d

OUTPUT_FILE_PATH +=KPSSynth.elf

OUTPUT_FILE_PATH_AS_ARGS +=KPSSynth.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

# AVR32/GNU C Compiler













./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG -DF_CPU=8000000 -DCYCLES_PER_US=((F_CPU+500000)/1000000) -DUSE_SMALL_BUFFER_STRUCTURE -DENABLE_UART0_RX -DUART_RX_BUFFER_SIZE=0x0020 -DMIDI_BUFFERED_NOTES -DSPI_USE_TIMER0  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=attiny2313a -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

_AVRLib/%.o: ../_AVRLib/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG -DF_CPU=8000000 -DCYCLES_PER_US=((F_CPU+500000)/1000000) -DUSE_SMALL_BUFFER_STRUCTURE -DENABLE_UART0_RX -DUART_RX_BUFFER_SIZE=0x0020 -DMIDI_BUFFERED_NOTES -DSPI_USE_TIMER0  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=attiny2313a -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

_BAPlib/%.o: ../_BAPlib/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG -DF_CPU=8000000 -DCYCLES_PER_US=((F_CPU+500000)/1000000) -DUSE_SMALL_BUFFER_STRUCTURE -DENABLE_UART0_RX -DUART_RX_BUFFER_SIZE=0x0020 -DMIDI_BUFFERED_NOTES -DSPI_USE_TIMER0  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=attiny2313a -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
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
	@echo Invoking: AVR/GNU Linker : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="KPSSynth.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=attiny2313a  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "KPSSynth.elf" "KPSSynth.hex"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "KPSSynth.elf" "KPSSynth.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "KPSSynth.elf" > "KPSSynth.lss"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "KPSSynth.elf" "KPSSynth.srec"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-size.exe" "KPSSynth.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "KPSSynth.elf" "KPSSynth.a" "KPSSynth.hex" "KPSSynth.lss" "KPSSynth.eep" "KPSSynth.map" "KPSSynth.srec" "KPSSynth.usersignatures"
	