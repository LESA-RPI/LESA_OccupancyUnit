################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/STM/workspace/BLE_MeshLightingPRFNode/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci/shci.c 

OBJS += \
./Middlewares/STM32_WPAN/interface/patterns/ble_thread/shci/shci.o 

C_DEPS += \
./Middlewares/STM32_WPAN/interface/patterns/ble_thread/shci/shci.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/STM32_WPAN/interface/patterns/ble_thread/shci/shci.o: C:/STM/workspace/BLE_MeshLightingPRFNode/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci/shci.c Middlewares/STM32_WPAN/interface/patterns/ble_thread/shci/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DUSE_STM32WBXX_NUCLEO -DENABLE_USART -DSTM32WB55xx -c -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Drivers/CMSIS/Include -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/BSP/P-NUCLEO-WB55.Nucleo -I../../Middlewares/ST/STM32_WPAN/ble/mesh/Inc -I../../Core/Inc -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Utilities/lpm/tiny_lpm -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/ble/mesh/MeshModel/Inc -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../STM32_WPAN/app -I../../Middlewares/ST/STM32_WPAN/ble -I"C:/STM/workspace/BLE_MeshLightingPRFNode/STM32CubeIDE/Application/Core" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-STM32_WPAN-2f-interface-2f-patterns-2f-ble_thread-2f-shci

clean-Middlewares-2f-STM32_WPAN-2f-interface-2f-patterns-2f-ble_thread-2f-shci:
	-$(RM) ./Middlewares/STM32_WPAN/interface/patterns/ble_thread/shci/shci.cyclo ./Middlewares/STM32_WPAN/interface/patterns/ble_thread/shci/shci.d ./Middlewares/STM32_WPAN/interface/patterns/ble_thread/shci/shci.o ./Middlewares/STM32_WPAN/interface/patterns/ble_thread/shci/shci.su

.PHONY: clean-Middlewares-2f-STM32_WPAN-2f-interface-2f-patterns-2f-ble_thread-2f-shci

