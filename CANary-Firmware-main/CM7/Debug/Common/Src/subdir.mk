################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
<<<<<<< HEAD
C:/Users/percy/OneDrive/Desktop/BFR/fixLights/CANary-Firmware-main/Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c 
=======
C:/Users/meghd/OneDrive/Computer/Documents/Projects/BFR/CANary/CANary-Firmware/Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c 
>>>>>>> be12bf661c97396046213c2422a57a1109e08fe3

OBJS += \
./Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.o 

C_DEPS += \
./Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.d 


# Each subdirectory must supply rules for building sources it contributes
<<<<<<< HEAD
Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.o: C:/Users/percy/OneDrive/Desktop/BFR/fixLights/CANary-Firmware-main/Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c Common/Src/subdir.mk
=======
Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.o: C:/Users/meghd/OneDrive/Computer/Documents/Projects/BFR/CANary/CANary-Firmware/Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c Common/Src/subdir.mk
>>>>>>> be12bf661c97396046213c2422a57a1109e08fe3
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Common-2f-Src

clean-Common-2f-Src:
	-$(RM) ./Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.cyclo ./Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.d ./Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.o ./Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.su

.PHONY: clean-Common-2f-Src

