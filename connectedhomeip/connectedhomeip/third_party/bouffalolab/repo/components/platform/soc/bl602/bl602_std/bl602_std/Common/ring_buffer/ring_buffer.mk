SUB_MODULE_DIR:= $(MODULE_DIR)/ring_buffer
SUB_MODULE_OUT_DIR:= $(MODULE_OUT_DIR)/ring_buffer

SUB_MODULE_SRC_DIR := $(SUB_MODULE_DIR)

COMMON_INCLUDE += -I $(MODULE_DIR)/ring_buffer \
                  
ring_buffer_sources := ring_buffer.c

ring_buffer_objs := $(addprefix $(SUB_MODULE_OUT_DIR)/, $(subst .c,.o,$(ring_buffer_sources)))


common_objs_target += $(ring_buffer_objs)

$(SUB_MODULE_OUT_DIR)/%.o:$(SUB_MODULE_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "cc $<"
	$(AT)$(CC) -c $(GLOBAL_CFLAGS) $(COMMON_CFLAGS) $(GLOBAL_INCLUDE) $(COMMON_INCLUDE) $< -o $@
