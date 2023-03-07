SUB_MODULE_DIR:= $(MODULE_DIR)/cipher_suite
SUB_MODULE_OUT_DIR:= $(MODULE_OUT_DIR)/cipher_suite

SUB_MODULE_SRC_DIR := $(SUB_MODULE_DIR)/src
CIPHER_SUITE_INC := -I$(SUB_MODULE_DIR)/inc

CIPHER_SUITE_CFLAGS :=

COMMON_INCLUDE += -I $(MODULE_DIR)/cipher_suite/inc

cipher_suite_sources :=   bflb_crypt.c bflb_hash.c bflb_ecdsa.c bflb_dsa.c

cipher_suite_objs := $(addprefix $(SUB_MODULE_OUT_DIR)/, $(subst .c,.o,$(cipher_suite_sources)))

bsp_common_objs_target += $(cipher_suite_objs)
                      
$(SUB_MODULE_OUT_DIR)/%.o:$(SUB_MODULE_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "cc $<"
	$(AT)$(CC) -c $(GLOBAL_CFLAGS) $(EXTERNAL_CFLAGS) $(BSP_CFLAGS) $(CIPHER_SUITE_CFLAGS) $(GLOBAL_INCLUDE) $(COMMON_INCLUDE) \
    $(BSP_INCLUDES) $(CIPHER_SUITE_INC) $< -o $@
