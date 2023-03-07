
ifeq (,$(shell which arm-none-eabi-gcc))
ifndef ARM_GCC_LOCATION
$(error "No arm-none-eabi-gcc in $(PATH). Define ARM_GCC_LOCATION to locate ARM GCC compiler")
else
PATH += $(ARM_GCC_LOCATION)
endif
endif

ifndef STM_BOARD
$(info STM_BOARD is not defined. Using default value of 'nucleo-f411re')
$(info Available STM32 boards:)
$(foreach d,$(notdir $(wildcard $(RSI_SDK_PATH)/platforms/stm32/boards/*)),$(info - $d))
STM_BOARD =nucleo-f411re
endif

PROG_EXTENSION :=.elf
VALID_BUSES=spi

CFLAGS += -D RSI_SPI_INTERFACE
 
LINKER_FLAGS += -Wl,--gc-sections --specs=nosys.specs

rm=rm -f
CC=arm-none-eabi-gcc
AR=arm-none-eabi-ar

SDK_FEATURES += stm32 freertos

stm32_SOURCES += $(RSI_SDK_PATH)/platforms/stm32/hal/rsi_hal_mcu_spi.c \
                 $(RSI_SDK_PATH)/platforms/stm32/hal/rsi_hal_mcu_interrupt.c \
                 $(RSI_SDK_PATH)/platforms/stm32/hal/rsi_hal_mcu_ioports.c \
                 $(RSI_SDK_PATH)/platforms/stm32/hal/rsi_hal_mcu_timer.c \
                 $(RSI_SDK_PATH)/platforms/stm32/hal/rsi_hal_mcu_platform_init.c \
                 $(RSI_SDK_PATH)/platforms/stm32/hal/rsi_hal_mcu_com_port.c \
                 $(RSI_SDK_PATH)/platforms/stm32/drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
                 $(RSI_SDK_PATH)/platforms/stm32/drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
                 $(RSI_SDK_PATH)/platforms/stm32/drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
                 $(RSI_SDK_PATH)/platforms/stm32/drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
                 $(RSI_SDK_PATH)/platforms/stm32/drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
                 $(RSI_SDK_PATH)/platforms/stm32/drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
                 $(RSI_SDK_PATH)/platforms/stm32/drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
                 $(RSI_SDK_PATH)/platforms/stm32/drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
                 $(RSI_SDK_PATH)/platforms/stm32/drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
                 $(RSI_SDK_PATH)/platforms/stm32/core/src/system_stm32f4xx.c

stm32_CFLAGS += -I $(RSI_SDK_PATH)/platforms/stm32/core/inc \
                -I $(RSI_SDK_PATH)/platforms/stm32/drivers/STM32F4xx_HAL_Driver/Inc \
                -I $(RSI_SDK_PATH)/platforms/stm32/drivers/CMSIS/Device/ST/STM32F4xx/Include \
                -I $(RSI_SDK_PATH)/platforms/stm32/drivers/CMSIS/Core/Include \
                -I $(RSI_SDK_PATH)/platforms/stm32/boards/$(STM_BOARD)
                
# Define the specific chip details
stm32_CFLAGS += -D USE_HAL_DRIVER \
                -D RSI_SPI_INTERFACE \
                -D RSI_WITH_OS \
                -D RSI_SPI_HIGH_SPEED_ENABLE

#stm32_CFLAGS += -D STM32F411xE
stm32_CFLAGS += -D STM32F429xx
stm32_CFLAGS += -mcpu=cortex-m4 -mthumb -mno-thumb-interwork -mfloat-abi=softfp -mfpu=auto -ffunction-sections -fdata-sections -Os

stm32: all


