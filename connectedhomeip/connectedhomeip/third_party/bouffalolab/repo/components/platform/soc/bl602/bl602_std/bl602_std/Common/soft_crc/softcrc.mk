SUB_MODULE_DIR:= $(MODULE_DIR)/soft_crc
SUB_MODULE_OUT_DIR:= $(MODULE_OUT_DIR)/soft_crc
SUB_MODULE_SRC_DIR := $(SUB_MODULE_DIR)


PLATFORM_PRINT_CFLAGS :=

PLATFORM_PRINT_INCLUDE := -I$(SUB_MODULE_DIR)/

softcrc_sources :=  softcrc.c

softcrc_objs := $(addprefix $(SUB_MODULE_OUT_DIR)/, $(subst .c,.o,$(softcrc_sources)))

common_objs_target += $(softcrc_objs)
                      
$(SUB_MODULE_OUT_DIR)/%.o:$(SUB_MODULE_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "cc $<"
	$(AT)$(CC) -c $(GLOBAL_CFLAGS) $(EXTERNAL_CFLAGS) $(PLATFORM_PRINT_CFLAGS) \
	$(GLOBAL_INCLUDE)  $(PLATFORM_PRINT_INCLUDE) $< -o $@
