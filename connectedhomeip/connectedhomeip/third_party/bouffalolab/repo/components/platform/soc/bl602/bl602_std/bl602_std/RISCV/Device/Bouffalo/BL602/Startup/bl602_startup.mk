# Copyright (C) 2016-2020, BouffaloLab Ltd.
# All Rights Reserved.

# source files and objs output dir
LOCAL_SRCS_FILE:=  system_bl602.c GCC/start_load.c interrupt.c

STARTUP_MODULE_DIR:= $(MODULE_DIR)/RISCV/Device/Bouffalo/$(DEVICE)/Startup
STARTUP_MODULE_OUT_DIR:= $(TARGET_OUT_PATH)/startup

# CFLAGS and included dirs for startup
STARTUP_CFLAGS:=
STARTUP_INCLUDE:=   -I$(MODULE_DIR)/RISCV/Device/Bouffalo/$(DEVICE)/Startup

LOCAL_ASM_FILE:= GCC/entry.S

startup_objs:= $(addprefix $(STARTUP_MODULE_OUT_DIR)/,$(subst .c,.o,$(LOCAL_SRCS_FILE))) 
startup_objs+= $(addprefix $(STARTUP_MODULE_OUT_DIR)/,$(subst .S,.o,$(LOCAL_ASM_FILE)))

$(STARTUP_MODULE_OUT_DIR)/%.o:$(STARTUP_MODULE_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "cc $<"
	$(AT)$(CC) -c $(GLOBAL_CFLAGS) $(STARTUP_CFLAGS) $(GLOBAL_INCLUDE) $(COMMON_INCLUDE) $(STARTUP_INCLUDE) $< -o $@
    
$(STARTUP_MODULE_OUT_DIR)/%.o:$(STARTUP_MODULE_DIR)/%.S
	@mkdir -p $(dir $@)
	@echo "cc $<"
	$(AT)$(CC) -c $(GLOBAL_CFLAGS) $(STARTUP_CFLAGS) $(GLOBAL_INCLUDE) $(COMMON_INCLUDE) $(STARTUP_INCLUDE) $< -o $@

base_objects += $(startup_objs)

.PHONY: startup
startup: $(startup_objs)
	@echo  "startup_objs is $(startup_objs)"
