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
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DUSE_STM32WBXX_NUCLEO -DDEBUG -DSTM32WB55xx -c -I../../Core/Inc -I../../STM32_WPAN/App -I../../../../../../../Drivers/STM32WBxx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../../../../../../../Utilities/lpm/tiny_lpm -I../../../../../../../Middlewares/ST/STM32_WPAN -I../../../../../../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I../../../../../../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I../../../../../../../Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -I../../../../../../../Middlewares/ST/STM32_WPAN/utilities -I../../../../../../../Middlewares/ST/STM32_WPAN/ble/core -I../../../../../../../Middlewares/ST/STM32_WPAN/ble/core/auto -I../../../../../../../Middlewares/ST/STM32_WPAN/ble/core/template -I../../../../../../../Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../../../../../../../Middlewares/ST/STM32_WPAN/ble/svc/Src -I../../../../../../../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../../../../../../../Utilities/sequencer -I../../../../../../../Middlewares/ST/STM32_WPAN/ble -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/BSP/P-NUCLEO-WB55.Nucleo -I../../tiny_lpm -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Drivers/STM32WBxx_HAL_Driver/Inc" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Drivers/STM32WBxx_HAL_Driver/Inc/Legacy" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Utilities/lpm/tiny_lpm" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Middlewares/ST/STM32_WPAN" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Middlewares/ST/STM32_WPAN/utilities" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Middlewares/ST/STM32_WPAN/ble/core" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Middlewares/ST/STM32_WPAN/ble/core/auto" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Middlewares/ST/STM32_WPAN/ble/core/template" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Middlewares/ST/STM32_WPAN/ble/svc/Inc" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Middlewares/ST/STM32_WPAN/ble/svc/Src" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Drivers/CMSIS/Device/ST/STM32WBxx/Include" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Utilities/sequencer" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Middlewares/ST/STM32_WPAN/ble" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Drivers/CMSIS/Include" -I../../USB_Device/App -I../../USB_Device/Target -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/Junjie Zhai/STM32Cube/Repository/STM32Cube_FW_WB_V1.18.0/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"D:/version1/stm32cube_fw_wb_v1180/STM32Cube_FW_WB_V1.18.0/Projects/P-NUCLEO-WB55.Nucleo/Applications/BLE/BLE_p2pServer/STM32CubeIDE/Drivers/BSP/TCS3430" -I"D:/version1/stm32cube_fw_wb_v1180/STM32Cube_FW_WB_V1.18.0/Projects/P-NUCLEO-WB55.Nucleo/Applications/BLE/BLE_p2pServer/STM32CubeIDE/Drivers/BSP/vl53l8cx" -I"D:/version1/stm32cube_fw_wb_v1180/STM32Cube_FW_WB_V1.18.0/Projects/P-NUCLEO-WB55.Nucleo/Applications/BLE/BLE_p2pServer/STM32CubeIDE/Drivers/BSP/vl53l8cx/porting" -I"D:/version1/stm32cube_fw_wb_v1180/STM32Cube_FW_WB_V1.18.0/Projects/P-NUCLEO-WB55.Nucleo/Applications/BLE/BLE_p2pServer/STM32CubeIDE/Drivers/BSP/vl53l8cx/modules" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-TCS3430

clean-Drivers-2f-BSP-2f-TCS3430:
	-$(RM) ./Drivers/BSP/TCS3430/TCS3430.cyclo ./Drivers/BSP/TCS3430/TCS3430.d ./Drivers/BSP/TCS3430/TCS3430.o ./Drivers/BSP/TCS3430/TCS3430.su

.PHONY: clean-Drivers-2f-BSP-2f-TCS3430

