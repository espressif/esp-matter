SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR ?= $(abspath ../../../../..)

include $(SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR)/imports.mak

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
AR = "$(IAR_ARMCOMPILER)/bin/iarchive"

OBJECTS = examples_platforms_utils_debug_uart.obj examples_platforms_utils_mac_frame.obj
NAME = libopenthread_platform_utils_mtd

CFLAGS = -I../../.. \
    -Ohz \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/examples/platforms \
    -I$SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/third_party/mbedtls \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/third_party/mbedtls/repo/include \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/include \
    -DDeviceFamily_CC13X2_CC26X2 \
    -DHAVE_CONFIG_H \
    -D_BSD_SOURCE=1 \
    -D_DEFAULT_SOURCE=1 \
    --debug \
    --silent \
    -e \
    --aeabi \
    --thumb \
    --diag_suppress=Pa050 \
    -DMBEDTLS_CONFIG_FILE="mbedtls-config-cc13x2_26x2.h" \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/ti/thread/cc13x2_26x2/libmbedcrypto/config \
    -DOPENTHREAD_MTD=1 \
    -DOPENTHREAD_CONFIG_FILE="openthread-config-cc13x2_26x2-mtd.h" \
    -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="openthread-core-cc13x2_26x2-config.h" \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/ti/thread/cc13x2_26x2/libopenthread_mtd/config \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/src/core \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/src \
    --cpu=Cortex-M4F \
    --vla

CPPFLAGS = --c++ \
    -I../../.. \
    -Ohz \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/examples/platforms \
    -I$SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/third_party/mbedtls \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/third_party/mbedtls/repo/include \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/include \
    -DDeviceFamily_CC13X2_CC26X2 \
    -DHAVE_CONFIG_H \
    -D_BSD_SOURCE=1 \
    -D_DEFAULT_SOURCE=1 \
    --debug \
    --silent \
    -e \
    --aeabi \
    --thumb \
    --diag_suppress=Pa050 \
    -DMBEDTLS_CONFIG_FILE="mbedtls-config-cc13x2_26x2.h" \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/ti/thread/cc13x2_26x2/libmbedcrypto/config \
    -DOPENTHREAD_MTD=1 \
    -DOPENTHREAD_CONFIG_FILE="openthread-config-cc13x2_26x2-mtd.h" \
    -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="openthread-core-cc13x2_26x2-config.h" \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/ti/thread/cc13x2_26x2/libopenthread_mtd/config \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/src/core \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/src \
    --cpu=Cortex-M4F

all: $(NAME).a

examples_platforms_utils_debug_uart.obj: OPENTHREAD_MAKE_PATH/examples/platforms/utils/debug_uart.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

examples_platforms_utils_mac_frame.obj: OPENTHREAD_MAKE_PATH/examples/platforms/utils/mac_frame.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< -o $@

$(NAME).a: $(OBJECTS)
	@ echo creating library $@
	@ $(AR) --create $@ $^

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).a > $(DEVNULL) 2>&1
