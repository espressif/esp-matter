#
# Component Makefile
#
CRYPTOAUTHLIB_DIR := cryptoauthlib/lib

COMPONENT_SRCDIRS := $(CRYPTOAUTHLIB_DIR)/atcacert \
                     $(CRYPTOAUTHLIB_DIR)/calib \
                     $(CRYPTOAUTHLIB_DIR)/crypto \
                     $(CRYPTOAUTHLIB_DIR)/crypto/hashes \
                     $(CRYPTOAUTHLIB_DIR)/host \
                     $(CRYPTOAUTHLIB_DIR)/mbedtls \
                     $(CRYPTOAUTHLIB_DIR)/../app/tng \
                     $(CRYPTOAUTHLIB_DIR) \
                     port

COMPONENT_OBJS := $(foreach compsrcdir,$(COMPONENT_SRCDIRS),$(patsubst %.c,%.o,$(wildcard $(COMPONENT_PATH)/$(compsrcdir)/*.c))) \
                  $(CRYPTOAUTHLIB_DIR)/hal/atca_hal.o \
                  $(CRYPTOAUTHLIB_DIR)/hal/hal_freertos.o \
                  $(CRYPTOAUTHLIB_DIR)/../third_party/hal/esp32/hal_esp32_timer.o \
                  $(CRYPTOAUTHLIB_DIR)/../third_party/hal/esp32/hal_esp32_i2c.o

# Make relative by removing COMPONENT_PATH from all found object paths
COMPONENT_OBJS := $(patsubst $(COMPONENT_PATH)/%,%,$(COMPONENT_OBJS))

# Don't include the default interface configurations from cryptoauthlib
COMPONENT_OBJEXCLUDE := $(CRYPTOAUTHLIB_DIR)/atca_cfgs.o

# Add the hal directory back in for source search paths
COMPONENT_SRCDIRS += $(CRYPTOAUTHLIB_DIR)/hal \
                     $(CRYPTOAUTHLIB_DIR)/../third_party/hal/esp32

COMPONENT_ADD_INCLUDEDIRS := $(CRYPTOAUTHLIB_DIR) $(CRYPTOAUTHLIB_DIR)/hal $(CRYPTOAUTHLIB_DIR)/../app/tng port

COMPONENT_PRIV_INCLUDEDIRS := port/include

# Library requires some global defines
CFLAGS+=-DESP32 -Wno-pointer-sign
