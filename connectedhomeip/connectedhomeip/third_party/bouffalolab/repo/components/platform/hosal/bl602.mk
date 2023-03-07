# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += bl602_hal platform_hal

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS +=

## This component's src
COMPONENT_SRCS += bl602_hal/bl_uart.c \
                  bl602_hal/bl_chip.c \
                  bl602_hal/bl_cks.c \
                  bl602_hal/bl_sys.c \
                  bl602_hal/bl_sys_cli.c \
                  bl602_hal/bl_dma.c \
                  bl602_hal/bl_irq.c \
                  bl602_hal/bl_sec.c \
                  bl602_hal/bl_ir.c \
                  bl602_hal/bl_boot2.c \
                  bl602_hal/bl_timer.c \
                  bl602_hal/bl_hbn.c \
                  bl602_hal/bl_efuse.c \
                  bl602_hal/bl_flash.c \
                  bl602_hal/bl_gpio.c \
                  bl602_hal/bl_gpio_cli.c \
                  bl602_hal/hal_gpio.c \
                  bl602_hal/hal_button.c \
                  bl602_hal/bl_pwm.c \
                  bl602_hal/bl_sec_aes.c \
                  bl602_hal/bl_wifi.c \
                  bl602_hal/bl_wdt.c \
                  bl602_hal/bl_wdt_cli.c \
                  bl602_hal/bl_clocktree.c \
                  bl602_hal/hal_boot2.c \
                  bl602_hal/hal_sys.c \
                  bl602_hal/hal_board.c \
                  bl602_hal/hal_ir.c \
                  bl602_hal/bl_adc.c \
                  bl602_hal/bl_dac_audio.c \
                  bl602_hal/bl_i2c.c \
                  bl602_hal/bl_pm.c \
                  bl602_hal/bl_pds.c \
                  bl602_hal/hosal_pwm.c \
                  bl602_hal/hal_pds.c \
                  bl602_hal/hosal_rng.c \
                  bl602_hal/bl_rtc.c \
                  bl602_hal/hal_hbn.c \
                  bl602_hal/hal_hbnram.c \
                  bl602_hal/hosal_rtc.c \
                  bl602_hal/hosal_gpio.c \
                  bl602_hal/hosal_adc.c \
                  bl602_hal/hosal_spi.c \
                  bl602_hal/hal_hwtimer.c \
                  bl602_hal/hal_wifi.c \
                  bl602_hal/hosal_wdg.c \
                  platform_hal/platform_hal_device.cpp \
                  bl602_hal/hosal_uart.c \
                  bl602_hal/hosal_dma.c \
                  bl602_hal/hosal_flash.c \
                  bl602_hal/hosal_dac.c \
                  bl602_hal/hosal_i2c.c \
                  bl602_hal/hosal_ota.c \
                  bl602_hal/hosal_timer.c \
                  bl602_hal/hosal_efuse.c \
                  sec_common/bl_sec_common.c \
                  sec_common/bl_sec_sha.c \
                  sec_common/bl_sec_pka.c \
                  sec_common/bl_sec_aes.c \

COMPONENT_SRCDIRS += bl602_hal platform_hal sec_common

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_OBJS := $(patsubst %.cpp,%.o, $(COMPONENT_OBJS))

##
ifeq ($(CONFIG_BT),1)
CPPFLAGS += -DCFG_BLE_ENABLE
else
endif

CPPFLAGS += -DARCH_RISCV -DBFLB_CRYPT_HARDWARE
ifndef CONFIG_USE_STD_DRIVER
CPPFLAGS += -DBL602_USE_HAL_DRIVER
endif

ifeq ($(CONFIG_USE_XTAL32K),1)
CFLAGS += -DCFG_USE_XTAL32K
endif

ifeq ($(CONF_ENABLE_COREDUMP),1)
CPPFLAGS += -DSYS_ENABLE_COREDUMP
endif

ifeq ($(CONFIG_BL602_USE_1M_FLASH),1)
CPPFLAGS += -DCONF_BL602_USE_1M_FLASH
endif
