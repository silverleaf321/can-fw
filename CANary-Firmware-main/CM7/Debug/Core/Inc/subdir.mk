################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Inc/FT81x.c \
../Core/Inc/display.c \
../Core/Inc/ledUtil.c \
../Core/Inc/xbee_platform_stm32.c \
../Core/Inc/xbee_serial_stm32.c 

OBJS += \
./Core/Inc/FT81x.o \
./Core/Inc/display.o \
./Core/Inc/ledUtil.o \
./Core/Inc/xbee_platform_stm32.o \
./Core/Inc/xbee_serial_stm32.o 

C_DEPS += \
./Core/Inc/FT81x.d \
./Core/Inc/display.d \
./Core/Inc/ledUtil.d \
./Core/Inc/xbee_platform_stm32.d \
./Core/Inc/xbee_serial_stm32.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Inc/%.o Core/Inc/%.su Core/Inc/%.cyclo: ../Core/Inc/%.c Core/Inc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Inc

clean-Core-2f-Inc:
	-$(RM) ./Core/Inc/FT81x.cyclo ./Core/Inc/FT81x.d ./Core/Inc/FT81x.o ./Core/Inc/FT81x.su ./Core/Inc/display.cyclo ./Core/Inc/display.d ./Core/Inc/display.o ./Core/Inc/display.su ./Core/Inc/ledUtil.cyclo ./Core/Inc/ledUtil.d ./Core/Inc/ledUtil.o ./Core/Inc/ledUtil.su ./Core/Inc/xbee_platform_stm32.cyclo ./Core/Inc/xbee_platform_stm32.d ./Core/Inc/xbee_platform_stm32.o ./Core/Inc/xbee_platform_stm32.su ./Core/Inc/xbee_serial_stm32.cyclo ./Core/Inc/xbee_serial_stm32.d ./Core/Inc/xbee_serial_stm32.o ./Core/Inc/xbee_serial_stm32.su

.PHONY: clean-Core-2f-Inc

