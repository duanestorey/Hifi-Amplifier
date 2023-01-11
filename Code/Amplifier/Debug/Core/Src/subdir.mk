################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/freertos.c \
../Core/Src/main.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c 

CPP_SRCS += \
../Core/Src/Amplifier.cpp \
../Core/Src/AudioCore.cpp \
../Core/Src/DAC.cpp \
../Core/Src/DACPCM1681.cpp \
../Core/Src/Display.cpp \
../Core/Src/DolbyDecoder.cpp \
../Core/Src/DolbyDecoderSTA310.cpp \
../Core/Src/I2C.cpp \
../Core/Src/I2CDevice.cpp \
../Core/Src/LED.cpp \
../Core/Src/UI.cpp 

C_DEPS += \
./Core/Src/freertos.d \
./Core/Src/main.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d 

OBJS += \
./Core/Src/Amplifier.o \
./Core/Src/AudioCore.o \
./Core/Src/DAC.o \
./Core/Src/DACPCM1681.o \
./Core/Src/Display.o \
./Core/Src/DolbyDecoder.o \
./Core/Src/DolbyDecoderSTA310.o \
./Core/Src/I2C.o \
./Core/Src/I2CDevice.o \
./Core/Src/LED.o \
./Core/Src/UI.o \
./Core/Src/freertos.o \
./Core/Src/main.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o 

CPP_DEPS += \
./Core/Src/Amplifier.d \
./Core/Src/AudioCore.d \
./Core/Src/DAC.d \
./Core/Src/DACPCM1681.d \
./Core/Src/Display.d \
./Core/Src/DolbyDecoder.d \
./Core/Src/DolbyDecoderSTA310.d \
./Core/Src/I2C.d \
./Core/Src/I2CDevice.d \
./Core/Src/LED.d \
./Core/Src/UI.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su: ../Core/Src/%.cpp Core/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Core/Src/%.o Core/Src/%.su: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Core/Src/main.o: ../Core/Src/main.c Core/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Amplifier.d ./Core/Src/Amplifier.o ./Core/Src/Amplifier.su ./Core/Src/AudioCore.d ./Core/Src/AudioCore.o ./Core/Src/AudioCore.su ./Core/Src/DAC.d ./Core/Src/DAC.o ./Core/Src/DAC.su ./Core/Src/DACPCM1681.d ./Core/Src/DACPCM1681.o ./Core/Src/DACPCM1681.su ./Core/Src/Display.d ./Core/Src/Display.o ./Core/Src/Display.su ./Core/Src/DolbyDecoder.d ./Core/Src/DolbyDecoder.o ./Core/Src/DolbyDecoder.su ./Core/Src/DolbyDecoderSTA310.d ./Core/Src/DolbyDecoderSTA310.o ./Core/Src/DolbyDecoderSTA310.su ./Core/Src/I2C.d ./Core/Src/I2C.o ./Core/Src/I2C.su ./Core/Src/I2CDevice.d ./Core/Src/I2CDevice.o ./Core/Src/I2CDevice.su ./Core/Src/LED.d ./Core/Src/LED.o ./Core/Src/LED.su ./Core/Src/UI.d ./Core/Src/UI.o ./Core/Src/UI.su ./Core/Src/freertos.d ./Core/Src/freertos.o ./Core/Src/freertos.su ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su

.PHONY: clean-Core-2f-Src

