# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += BSP_Driver \
                             BSP_Driver/regs \
                             BSP_Driver/risc-v/Core/Include \
                             BSP_Driver/startup \
                             BSP_Driver/std_drv/inc \
                             BSP_Common/platform \
                             MCU_Common/device \
                             MCU_Common/list \
                             MCU_Common/memheap \
                             MCU_Common/misc \
                             MCU_Common/partition \
                             MCU_Common/ring_buffer \
                             MCU_Common/soft_crc \
                             cipher_suite/inc \

ifeq ($(CONFIG_BL702_USE_BSP_STARTUP),1)
COMPONENT_ADD_INCLUDEDIRS += BSP_Board
COMPONENT_ADD_INCLUDEDIRS += BSP_Driver/hal_drv/default_config
COMPONENT_ADD_INCLUDEDIRS += BSP_Driver/hal_drv/inc
endif


## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := 


## This component's src 
COMPONENT_SRCS := BSP_Driver/std_drv/src/bl702_acomp.c \
                  BSP_Driver/std_drv/src/bl702_adc.c \
                  BSP_Driver/std_drv/src/bl702_aon.c \
                  BSP_Driver/std_drv/src/bl702_cam.c \
                  BSP_Driver/std_drv/src/bl702_clock.c \
                  BSP_Driver/std_drv/src/bl702_common.c \
                  BSP_Driver/std_drv/src/bl702_dac.c \
                  BSP_Driver/std_drv/src/bl702_dma.c \
                  BSP_Driver/std_drv/src/bl702_ef_ctrl.c \
                  BSP_Driver/std_drv/src/bl702_glb.c \
                  BSP_Driver/std_drv/src/bl702_hbn.c \
                  BSP_Driver/std_drv/src/bl702_i2c.c \
                  BSP_Driver/std_drv/src/bl702_i2s.c \
                  BSP_Driver/std_drv/src/bl702_ir.c \
                  BSP_Driver/std_drv/src/bl702_l1c.c \
                  BSP_Driver/std_drv/src/bl702_mjpeg.c \
                  BSP_Driver/std_drv/src/bl702_pds.c \
                  BSP_Driver/std_drv/src/bl702_psram.c \
                  BSP_Driver/std_drv/src/bl702_pwm.c \
                  BSP_Driver/std_drv/src/bl702_qdec.c \
                  BSP_Driver/std_drv/src/bl702_sec_dbg.c \
                  BSP_Driver/std_drv/src/bl702_sec_eng.c \
                  BSP_Driver/std_drv/src/bl702_sf_cfg.c \
                  BSP_Driver/std_drv/src/bl702_sf_cfg_ext.c \
                  BSP_Driver/std_drv/src/bl702_sf_ctrl.c \
                  BSP_Driver/std_drv/src/bl702_sflash.c \
                  BSP_Driver/std_drv/src/bl702_sflash_ext.c \
                  BSP_Driver/std_drv/src/bl702_spi.c \
                  BSP_Driver/std_drv/src/bl702_timer.c \
                  BSP_Driver/std_drv/src/bl702_uart.c \
                  BSP_Driver/std_drv/src/bl702_xip_sflash.c \
                  BSP_Driver/std_drv/src/bl702_xip_sflash_ext.c \
                  MCU_Common/misc/misc.c \
                  MCU_Common/ring_buffer/ring_buffer.c \
                  MCU_Common/soft_crc/softcrc.c \
                  cipher_suite/src/bflb_crypt.c \
                  cipher_suite/src/bflb_dsa.c \
                  cipher_suite/src/bflb_ecdsa.c \
                  cipher_suite/src/bflb_hash.c \

ifeq ($(CONFIG_BL702_USE_ROM_DRIVER),1)
COMPONENT_SRCS += BSP_Driver/std_drv/src/bl702_romapi.c
CPPFLAGS += -DBFLB_USE_ROM_DRIVER
endif

ifeq ($(CONFIG_BL702_USE_USB_DRIVER),1)
COMPONENT_SRCS += BSP_Driver/std_drv/src/bl702_usb.c
endif

ifeq ($(CONFIG_BL702_USE_BSP_STARTUP),1)
COMPONENT_SRCS += BSP_Driver/startup/interrupt.c
COMPONENT_SRCS += BSP_Driver/startup/system_bl702.c
COMPONENT_SRCS += BSP_Driver/startup/GCC/entry.S
COMPONENT_SRCS += BSP_Driver/startup/GCC/start_load.c
COMPONENT_SRCS += BSP_Common/platform/bflb_platform.c
COMPONENT_SRCS += MCU_Common/device/drv_device.c
COMPONENT_SRCS += MCU_Common/memheap/drv_mmheap.c
COMPONENT_SRCS += MCU_Common/partition/partition.c
COMPONENT_SRCS += BSP_Board/board.c
HAL_DRV_SRCS := $(notdir $(wildcard $(BL60X_SDK_PATH)/components/platform/soc/bl702/bl702_std/BSP_Driver/hal_drv/src/*.c))
COMPONENT_SRCS += $(addprefix BSP_Driver/hal_drv/src/, $(HAL_DRV_SRCS))
CPPFLAGS += -Dbl706_iot
endif


COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_OBJS := $(patsubst %.S,%.o, $(COMPONENT_OBJS))


COMPONENT_SRCDIRS := BSP_Driver/startup \
                     BSP_Driver/startup/GCC \
                     BSP_Driver/std_drv/src \
                     BSP_Common/platform \
                     MCU_Common/device \
                     MCU_Common/list \
                     MCU_Common/memheap \
                     MCU_Common/misc \
                     MCU_Common/partition \
                     MCU_Common/ring_buffer \
                     MCU_Common/soft_crc \
                     cipher_suite/src \

ifeq ($(CONFIG_BL702_USE_BSP_STARTUP),1)
COMPONENT_SRCDIRS += BSP_Board
COMPONENT_SRCDIRS += BSP_Driver/hal_drv/src
endif


##
CPPFLAGS += -DARCH_RISCV -DBFLB_CRYPT_HARDWARE -DBFLB_PKA_HARDWARE
ifndef CONFIG_USE_STD_DRIVER
CPPFLAGS += -DBFLB_USE_HAL_DRIVER
endif
