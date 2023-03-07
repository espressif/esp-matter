SUB_MODULE_DIR:= $(MODULE_DIR)/xz
SUB_MODULE_OUT_DIR:= $(MODULE_OUT_DIR)/xz

SUB_MODULE_SRC_DIR := $(SUB_MODULE_DIR)

COMMON_INCLUDE += -I $(MODULE_DIR)/xz 
                  
xz_sources := xz_crc32.c xz_dec_lzma2.c xz_dec_stream.c xz_decompress.c xz_port.c

xz_objs := $(addprefix $(SUB_MODULE_OUT_DIR)/, $(subst .c,.o,$(xz_sources)))

common_objs_target += $(xz_objs)

$(SUB_MODULE_OUT_DIR)/%.o:$(SUB_MODULE_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "cc $<"
	$(AT)$(CC) -c $(GLOBAL_CFLAGS) $(COMMON_CFLAGS) $(GLOBAL_INCLUDE) $(COMMON_INCLUDE) $< -o $@