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
../_AVRLib/ \
../_BAPlib/


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../../../AVRlib/timer.c \
../../../BAPlib/WGM.c \
../KPSVoiceClock.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
_AVRLib/timer.o \
_BAPlib/WGM.o \
KPSVoiceClock.o

OBJS_AS_ARGS +=  \
_AVRLib/timer.o \
_BAPlib/WGM.o \
KPSVoiceClock.o

C_DEPS +=  \
_AVRLib/timer.d \
_BAPlib/WGM.d \
KPSVoiceClock.d

C_DEPS_AS_ARGS +=  \
_AVRLib/timer.d \
_BAPlib/WGM.d \
KPSVoiceClock.d

OUTPUT_FILE_PATH +=KPSVoiceClock.elf

OUTPUT_FILE_PATH_AS_ARGS +=KPSVoiceClock.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

# AVR32/GNU C Compiler







_AVRLib/%.o: ../../../AVRlib/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG -DF_CPU=8000000 -DCYCLES_PER_US=((F_CPU+500000)/1000000) -DUSE_SMALL_BUFFER_STRUCTURE  -UTIMER0OVERFLOW_INT -UTIMER1OVERFLOW_INT -UTIMER1OUTCOMPAREA_INT -UTIMER1OUTCOMPAREB_INT -UTIMER1INPUTCAPUTRE_INT -UTIMER2OVERFLOW_INT -UTIMER2OUTCOMPARE_INT  -I"../../.."  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=attiny25 -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

_BAPlib/%.o: ../../../BAPlib/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG -DF_CPU=8000000 -DCYCLES_PER_US=((F_CPU+500000)/1000000) -DUSE_SMALL_BUFFER_STRUCTURE  -UTIMER0OVERFLOW_INT -UTIMER1OVERFLOW_INT -UTIMER1OUTCOMPAREA_INT -UTIMER1OUTCOMPAREB_INT -UTIMER1INPUTCAPUTRE_INT -UTIMER2OVERFLOW_INT -UTIMER2OUTCOMPARE_INT  -I"../../.."  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=attiny25 -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG -DF_CPU=8000000 -DCYCLES_PER_US=((F_CPU+500000)/1000000) -DUSE_SMALL_BUFFER_STRUCTURE  -UTIMER0OVERFLOW_INT -UTIMER1OVERFLOW_INT -UTIMER1OUTCOMPAREA_INT -UTIMER1OUTCOMPAREB_INT -UTIMER1INPUTCAPUTRE_INT -UTIMER2OVERFLOW_INT -UTIMER2OUTCOMPARE_INT  -I"../../.."  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=attiny25 -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
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
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="KPSVoiceClock.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=attiny25  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "KPSVoiceClock.elf" "KPSVoiceClock.hex"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "KPSVoiceClock.elf" "KPSVoiceClock.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "KPSVoiceClock.elf" > "KPSVoiceClock.lss"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "KPSVoiceClock.elf" "KPSVoiceClock.srec"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-size.exe" "KPSVoiceClock.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "KPSVoiceClock.elf" "KPSVoiceClock.a" "KPSVoiceClock.hex" "KPSVoiceClock.lss" "KPSVoiceClock.eep" "KPSVoiceClock.map" "KPSVoiceClock.srec" "KPSVoiceClock.usersignatures"
	