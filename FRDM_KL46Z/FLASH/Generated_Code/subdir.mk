################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Generated_Code/Cpu.c \
../Generated_Code/FTM.c \
../Generated_Code/I2CFreedom.c \
../Generated_Code/LED_GREEN.c \
../Generated_Code/LED_RED.c \
../Generated_Code/PE_LDD.c \
../Generated_Code/SegLCD.c \
../Generated_Code/UART.c \
../Generated_Code/Vectors.c 

OBJS += \
./Generated_Code/Cpu.o \
./Generated_Code/FTM.o \
./Generated_Code/I2CFreedom.o \
./Generated_Code/LED_GREEN.o \
./Generated_Code/LED_RED.o \
./Generated_Code/PE_LDD.o \
./Generated_Code/SegLCD.o \
./Generated_Code/UART.o \
./Generated_Code/Vectors.o 

C_DEPS += \
./Generated_Code/Cpu.d \
./Generated_Code/FTM.d \
./Generated_Code/I2CFreedom.d \
./Generated_Code/LED_GREEN.d \
./Generated_Code/LED_RED.d \
./Generated_Code/PE_LDD.d \
./Generated_Code/SegLCD.d \
./Generated_Code/UART.d \
./Generated_Code/Vectors.d 


# Each subdirectory must supply rules for building sources it contributes
Generated_Code/%.o: ../Generated_Code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -I"C:\Freescale\KDS_3.0.0\eclipse\ProcessorExpert/lib/Kinetis/pdd/inc" -I"C:\Freescale\KDS_3.0.0\eclipse\ProcessorExpert/lib/Kinetis/iofiles" -I"C:/Users/Tingkai/Desktop/Freescale/KDS/FRDM_KL46Z/Sources" -I"C:/Users/Tingkai/Desktop/Freescale/KDS/FRDM_KL46Z/Generated_Code" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


