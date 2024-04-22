################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/vl53l8cx/modules/platform.c \
../Drivers/BSP/vl53l8cx/modules/vl53l8cx_api.c \
../Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_detection_thresholds.c \
../Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_motion_indicator.c \
../Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_xtalk.c 

OBJS += \
./Drivers/BSP/vl53l8cx/modules/platform.o \
./Drivers/BSP/vl53l8cx/modules/vl53l8cx_api.o \
./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_detection_thresholds.o \
./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_motion_indicator.o \
./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_xtalk.o 

C_DEPS += \
./Drivers/BSP/vl53l8cx/modules/platform.d \
./Drivers/BSP/vl53l8cx/modules/vl53l8cx_api.d \
./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_detection_thresholds.d \
./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_motion_indicator.d \
./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_xtalk.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/vl53l8cx/modules/%.o Drivers/BSP/vl53l8cx/modules/%.su Drivers/BSP/vl53l8cx/modules/%.cyclo: ../Drivers/BSP/vl53l8cx/modules/%.c Drivers/BSP/vl53l8cx/modules/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../Drivers/BSP/TCS3430 -I../Drivers/BSP/vl53l8cx/modules -I../Drivers/BSP/vl53l8cx -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-vl53l8cx-2f-modules

clean-Drivers-2f-BSP-2f-vl53l8cx-2f-modules:
	-$(RM) ./Drivers/BSP/vl53l8cx/modules/platform.cyclo ./Drivers/BSP/vl53l8cx/modules/platform.d ./Drivers/BSP/vl53l8cx/modules/platform.o ./Drivers/BSP/vl53l8cx/modules/platform.su ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_api.cyclo ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_api.d ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_api.o ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_api.su ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_detection_thresholds.cyclo ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_detection_thresholds.d ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_detection_thresholds.o ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_detection_thresholds.su ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_motion_indicator.cyclo ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_motion_indicator.d ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_motion_indicator.o ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_motion_indicator.su ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_xtalk.cyclo ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_xtalk.d ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_xtalk.o ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_xtalk.su

.PHONY: clean-Drivers-2f-BSP-2f-vl53l8cx-2f-modules

