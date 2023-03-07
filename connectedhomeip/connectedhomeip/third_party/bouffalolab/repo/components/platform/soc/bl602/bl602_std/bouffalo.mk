# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += bl602_std/StdDriver/Inc \
							 bl602_std/Device/Bouffalo/BL602/Peripherals \
							 bl602_std/RISCV/Device/Bouffalo/BL602/Startup \
							 bl602_std/RISCV/Core/Include \
							 bl602_std/Include \
							 bl602_std/Common/platform_print \
							 bl602_std/Common/soft_crc \
							 bl602_std/Common/partition \
							 bl602_std/Common/xz \
							 bl602_std/Common/cipher_suite/inc \
							 bl602_std/Common/ring_buffer \

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=							 


## This component's src 
COMPONENT_SRCS := bl602_std/StdDriver/Src/bl602_uart.c \
                  bl602_std/StdDriver/Src/bl602_adc.c \
                  bl602_std/StdDriver/Src/bl602_sec_eng.c \
                  bl602_std/StdDriver/Src/bl602_dma.c \
                  bl602_std/StdDriver/Src/bl602_common.c \
                  bl602_std/StdDriver/Src/bl602_glb.c \
                  bl602_std/StdDriver/Src/bl602_hbn.c \
                  bl602_std/StdDriver/Src/bl602_timer.c \
                  bl602_std/StdDriver/Src/bl602_aon.c \
                  bl602_std/StdDriver/Src/bl602_pds.c \
                  bl602_std/StdDriver/Src/bl602_pwm.c \
                  bl602_std/StdDriver/Src/bl602_l1c.c \
                  bl602_std/StdDriver/Src/bl602_ef_ctrl.c \
                  bl602_std/StdDriver/Src/bl602_ef_ctrl.c \
                  bl602_std/StdDriver/Src/bl602_mfg_efuse.c \
                  bl602_std/StdDriver/Src/bl602_mfg_flash.c \
                  bl602_std/StdDriver/Src/bl602_mfg_media.c \
                  bl602_std/StdDriver/Src/bl602_dac.c \
                  bl602_std/StdDriver/Src/bl602_ir.c \
                  bl602_std/StdDriver/Src/bl602_spi.c \
                  bl602_std/StdDriver/Src/bl602_i2c.c \
                  bl602_std/StdDriver/Src/bl602_sdu.c \
                  bl602_std/Common/soft_crc/softcrc.c \
                  bl602_std/Common/xz/xz_crc32.c \
                  bl602_std/Common/xz/xz_dec_lzma2.c \
                  bl602_std/Common/xz/xz_dec_stream.c \
                  bl602_std/Common/xz/xz_decompress.c \
                  bl602_std/Common/xz/xz_port.c \
                  bl602_std/Common/cipher_suite/src/bflb_crypt.c \
                  bl602_std/Common/cipher_suite/src/bflb_hash.c \
                  bl602_std/Common/cipher_suite/src/bflb_dsa.c \
                  bl602_std/Common/cipher_suite/src/bflb_ecdsa.c \
                  bl602_std/Common/platform_print/platform_device.c \
                  bl602_std/Common/platform_print/platform_gpio.c \
                  bl602_std/Common/ring_buffer/ring_buffer.c \
                  bl602_std/RISCV/Device/Bouffalo/BL602/Startup/interrupt.c
                  
ifeq ($(CONFIG_BL602_USE_ROM_DRIVER),0)
COMPONENT_SRCS += bl602_std/StdDriver/Src/bl602_sf_ctrl.c \
                  bl602_std/StdDriver/Src/bl602_sflash.c \
                  bl602_std/StdDriver/Src/bl602_sflash_ext.c \
                  bl602_std/StdDriver/Src/bl602_xip_sflash.c \
                  bl602_std/StdDriver/Src/bl602_sf_cfg.c \
                  bl602_std/StdDriver/Src/bl602_sf_cfg_ext.c \
                  bl602_std/StdDriver/Src/bl602_xip_sflash_ext.c \

else
COMPONENT_SRCS += bl602_std/StdDriver/Src/bl602_romapi.c \
                  bl602_std/StdDriver/Src/bl602_sflash_ext.c \
                  bl602_std/StdDriver/Src/bl602_sf_cfg_ext.c \
                  bl602_std/StdDriver/Src/bl602_xip_sflash_ext.c \

endif

ifeq ($(CONFIG_BL602_USE_BSP_PARTITION),1)
COMPONENT_SRCS += bl602_std/Common/partition/partition.c 
endif

ifeq ($(CONFIG_BL602_USE_BSP_STARTUP),1)
COMPONENT_SRCS += bl602_std/RISCV/Device/Bouffalo/BL602/Startup/GCC/entry.S
COMPONENT_SRCS += bl602_std/RISCV/Device/Bouffalo/BL602/Startup/GCC/start_load.c
COMPONENT_SRCS += bl602_std/RISCV/Device/Bouffalo/BL602/Startup/system_bl602.c
endif


COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_OBJS := $(patsubst %.S,%.o, $(COMPONENT_OBJS))

COMPONENT_SRCDIRS := bl602_std bl602_std/StdDriver/Src bl602_std/Common/soft_crc \
                     bl602_std/Common/cipher_suite/src bl602_std/Common/partition/src \
                     bl602_std/Common/xz bl602_std/Common/platform_print bl602_std/Common/ring_buffer \
                     bl602_std/RISCV/Device/Bouffalo/BL602/Startup \
                     bl602_std/RISCV/Device/Bouffalo/BL602/Startup/GCC

##
CPPFLAGS += -DARCH_RISCV -DBFLB_CRYPT_HARDWARE -DBFLB_PKA_HARDWARE -DSTDDRV_VERSION=096d971a96c12b5857abc7606bfd5ac1bf371a41
ifndef CONFIG_USE_STD_DRIVER
CPPFLAGS += -DBL602_USE_HAL_DRIVER
endif



