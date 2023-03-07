###################################################
# Sources
LWPRINTF_ROOT = middleware/third_party/lwprintf/
LWPRINTF_SRC = $(LWPRINTF_ROOT)/lwprintf/src/lwprintf
LWPRINTF_INC = $(LWPRINTF_ROOT)/lwprintf/src/include

C_FILES += $(LWPRINTF_SRC)/lwprintf.c

#################################################################################
# include path
CFLAGS 	+= -I$(SOURCE_DIR)/$(LWPRINTF_INC)
# config file
CFLAGS 	+= -I$(SOURCE_DIR)/$(LWPRINTF_ROOT)/config

