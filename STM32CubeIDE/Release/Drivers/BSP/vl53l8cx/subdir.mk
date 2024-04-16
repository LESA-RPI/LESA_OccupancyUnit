################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/vl53l8cx/vl53l8cx.c 

OBJS += \
./Drivers/BSP/vl53l8cx/vl53l8cx.o 

C_DEPS += \
./Drivers/BSP/vl53l8cx/vl53l8cx.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/vl53l8cx/%.o Drivers/BSP/vl53l8cx/%.su Drivers/BSP/vl53l8cx/%.cyclo: ../Drivers/BSP/vl53l8cx/%.c Drivers/BSP/vl53l8cx/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DUSE_STM32WBXX_NUCLEO -DENABLE_USART -DSTM32WB55xx -c -I../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../Drivers/CMSIS/Include -I../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../Drivers/BSP/P-NUCLEO-WB55.Nucleo -I../../Middlewares/ST/STM32_WPAN/ble/mesh/Inc -I../../Core/Inc -I../../Middlewares/ST/STM32_WPAN -I../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../Middlewares/ST/STM32_WPAN/utilities -I../../Utilities/lpm/tiny_lpm -I../../Utilities/sequencer -I../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../Middlewares/ST/STM32_WPAN/ble/core -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../Middlewares/ST/STM32_WPAN/ble/mesh/MeshModel/Inc -I../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../STM32_WPAN/app -I../../Middlewares/ST/STM32_WPAN/ble -I"D:/Research Projects/Occupancy/LESA_OccupancyUnit/STM32CubeIDE/Application/Core" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-vl53l8cx

clean-Drivers-2f-BSP-2f-vl53l8cx:
	-$(RM) ./Drivers/BSP/vl53l8cx/vl53l8cx.cyclo ./Drivers/BSP/vl53l8cx/vl53l8cx.d ./Drivers/BSP/vl53l8cx/vl53l8cx.o ./Drivers/BSP/vl53l8cx/vl53l8cx.su

.PHONY: clean-Drivers-2f-BSP-2f-vl53l8cx

