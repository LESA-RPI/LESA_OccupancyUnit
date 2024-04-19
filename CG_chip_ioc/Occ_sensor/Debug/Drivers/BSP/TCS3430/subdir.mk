################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/TCS3430/TCS3430.c 

OBJS += \
./Drivers/BSP/TCS3430/TCS3430.o 

C_DEPS += \
./Drivers/BSP/TCS3430/TCS3430.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/TCS3430/%.o Drivers/BSP/TCS3430/%.su Drivers/BSP/TCS3430/%.cyclo: ../Drivers/BSP/TCS3430/%.c Drivers/BSP/TCS3430/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-TCS3430

clean-Drivers-2f-BSP-2f-TCS3430:
	-$(RM) ./Drivers/BSP/TCS3430/TCS3430.cyclo ./Drivers/BSP/TCS3430/TCS3430.d ./Drivers/BSP/TCS3430/TCS3430.o ./Drivers/BSP/TCS3430/TCS3430.su

.PHONY: clean-Drivers-2f-BSP-2f-TCS3430

