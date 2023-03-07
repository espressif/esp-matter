#clear_vars
LOCAL_SRCS_FILE:=

MODULE_DIR:= BSP/StdDriver
SUBMODULE_OUT_DIR:= $(TARGET_OUT_PATH)/BSP/StdDriver
SUBMODULE_SRC_DIR := $(MODULE_DIR)/Src

STDDRIVER_CFLAGS:=
STDDRIVER_INCLUDE:=  -I $(MODULE_DIR)/Inc \

stddriver_sources := bl602_common.c bl602_uart.c bl602_dma.c bl602_glb.c bl602_timer.c bl602_ef_ctrl.c  \
                     bl602_aon.c bl602_hbn.c bl602_sec_eng.c bl602_sdu.c bl602_dac.c \
                     bl602_l1c.c bl602_pwm.c bl602_i2c.c bl602_sec_dbg.c bl602_ir.c bl602_spi.c \
                     bl602_pds.c bl602_tzc_sec.c bl602_adc.c bl602_acomp.c

ifeq ($(ROMAPI),y)
ifeq ($(ROMAPI_TEST),y)
	stddriver_sources += bl602_romapi.c
    STDDRIVER_CFLAGS += -DBL602_USE_ROM_DRIVER
else
    stddriver_sources += bl602_romapi.c bl602_sflash_ext.c bl602_sf_cfg_ext.c bl602_xip_sflash_ext.c
    STDDRIVER_CFLAGS += -DBL602_USE_ROM_DRIVER
endif
else
	stddriver_sources += bl602_sf_ctrl.c bl602_sflash.c bl602_sf_cfg.c  bl602_xip_sflash.c bl602_sflash_ext.c \
	 					 bl602_sf_cfg_ext.c bl602_xip_sflash_ext.c
endif

stddriver_objs := $(addprefix $(SUBMODULE_OUT_DIR)/, $(subst .c,.o,$(stddriver_sources)))
base_objects += $(stddriver_objs)

$(SUBMODULE_OUT_DIR)/%.o:$(SUBMODULE_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "cc $<"
	$(AT)$(CC) -c $(GLOBAL_CFLAGS) $(STDDRIVER_CFLAGS) $(GLOBAL_INCLUDE) $(COMMON_INCLUDE) $(STDDRIVER_INCLUDE) $< -o $@

.PHONY: stddriver
stddriver: $(stddriver_objs)
	@echo  "stddriver_objs is $(stddriver_objs)"
