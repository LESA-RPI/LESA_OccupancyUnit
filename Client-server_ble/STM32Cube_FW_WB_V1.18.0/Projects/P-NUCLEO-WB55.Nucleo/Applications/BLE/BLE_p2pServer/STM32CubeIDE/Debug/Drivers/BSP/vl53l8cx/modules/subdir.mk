################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/vl53l8cx/modules/vl53l8cx_api.c \
../Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_detection_thresholds.c \
../Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_motion_indicator.c \
../Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_xtalk.c 

OBJS += \
./Drivers/BSP/vl53l8cx/modules/vl53l8cx_api.o \
./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_detection_thresholds.o \
./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_motion_indicator.o \
./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_xtalk.o 

C_DEPS += \
./Drivers/BSP/vl53l8cx/modules/vl53l8cx_api.d \
./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_detection_thresholds.d \
./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_motion_indicator.d \
./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_xtalk.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/vl53l8cx/modules/%.o Drivers/BSP/vl53l8cx/modules/%.su Drivers/BSP/vl53l8cx/modules/%.cyclo: ../Drivers/BSP/vl53l8cx/modules/%.c Drivers/BSP/vl53l8cx/modules/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DUSE_STM32WBXX_NUCLEO -DDEBUG -DSTM32WB55xx -c -I../../Core/Inc -I../../STM32_WPAN/App -I../../../../../../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../../../../../../Utilities/lpm/tiny_lpm -I../../../../../../../Middlewares/ST/STM32_WPAN -I../../../../../../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../../../../../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../../../../../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../../../../../../Middlewares/ST/STM32_WPAN/utilities -I../../../../../../../Middlewares/ST/STM32_WPAN/ble/core -I../../../../../../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../../../../../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../../../../../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../../../../../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../../../../../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../../../../../../Utilities/sequencer -I../../../../../../../Middlewares/ST/STM32_WPAN/ble -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/BSP/P-NUCLEO-WB55.Nucleo -I"C:/stm32cube_fw_wb_v1180/STM32Cube_FW_WB_V1.18.0/Projects/P-NUCLEO-WB55.Nucleo/Applications/BLE/BLE_p2pServer/STM32CubeIDE/Drivers/BSP/TCS3430" -I"C:/stm32cube_fw_wb_v1180/STM32Cube_FW_WB_V1.18.0/Projects/P-NUCLEO-WB55.Nucleo/Applications/BLE/BLE_p2pServer/STM32CubeIDE/Drivers/BSP/vl53l8cx" -I"C:/stm32cube_fw_wb_v1180/STM32Cube_FW_WB_V1.18.0/Projects/P-NUCLEO-WB55.Nucleo/Applications/BLE/BLE_p2pServer/STM32CubeIDE/Drivers/BSP/vl53l8cx/porting" -I"C:/stm32cube_fw_wb_v1180/STM32Cube_FW_WB_V1.18.0/Projects/P-NUCLEO-WB55.Nucleo/Applications/BLE/BLE_p2pServer/STM32CubeIDE/Drivers/BSP/vl53l8cx/modules" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-vl53l8cx-2f-modules

clean-Drivers-2f-BSP-2f-vl53l8cx-2f-modules:
	-$(RM) ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_api.cyclo ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_api.d ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_api.o ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_api.su ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_detection_thresholds.cyclo ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_detection_thresholds.d ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_detection_thresholds.o ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_detection_thresholds.su ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_motion_indicator.cyclo ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_motion_indicator.d ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_motion_indicator.o ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_motion_indicator.su ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_xtalk.cyclo ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_xtalk.d ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_xtalk.o ./Drivers/BSP/vl53l8cx/modules/vl53l8cx_plugin_xtalk.su

.PHONY: clean-Drivers-2f-BSP-2f-vl53l8cx-2f-modules

