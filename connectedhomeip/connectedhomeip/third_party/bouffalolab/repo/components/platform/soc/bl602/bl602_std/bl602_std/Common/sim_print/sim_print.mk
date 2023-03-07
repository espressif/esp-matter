#
SUB_MODULE_DIR:= $(MODULE_DIR)/sim_print
SUB_MODULE_OUT_DIR:= $(MODULE_OUT_DIR)/sim_print

SUB_MODULE_SRC_DIR := $(SUB_MODULE_DIR)/

ifeq ($(DV_TEST),y)
COMMON_INCLUDE += -I $(MODULE_DIR)/sim_print
endif

sim_print_sources :=  

sim_print_objs := $(addprefix $(SUB_MODULE_OUT_DIR)/, $(subst .c,.o,$(sim_print_sources)))

$(SUB_MODULE_OUT_DIR)/%.o:$(SUB_MODULE_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "cc $<"
	$(AT)$(CC) -c $(GLOBAL_CFLAGS) $(COMMON_CFLAGS) $(GLOBAL_INCLUDE) $(COMMON_INCLUDE) $< -o $@
