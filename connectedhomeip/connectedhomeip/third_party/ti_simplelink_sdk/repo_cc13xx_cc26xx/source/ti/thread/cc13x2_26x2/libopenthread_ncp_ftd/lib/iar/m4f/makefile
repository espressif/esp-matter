SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR ?= $(abspath ../../../../..)

include $(SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR)/imports.mak

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
AR = "$(IAR_ARMCOMPILER)/bin/iarchive"

OBJECTS = src_ncp_changed_props_set.obj src_ncp_example_vendor_hook.obj src_ncp_ncp_base.obj src_ncp_ncp_base_dispatcher.obj src_ncp_ncp_base_ftd.obj src_ncp_ncp_base_mtd.obj src_ncp_ncp_base_radio.obj src_ncp_ncp_spi.obj src_ncp_ncp_uart.obj src_lib_hdlc_hdlc.obj src_lib_spinel_spinel.obj src_lib_spinel_spinel_buffer.obj src_lib_spinel_spinel_decoder.obj src_lib_spinel_spinel_encoder.obj
NAME = libopenthread_ncp_ftd

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
    -DOPENTHREAD_FTD=1 \
    -DOPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=1 \
    -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="openthread-core-cc13x2_26x2-config.h" \
    -DOPENTHREAD_CONFIG_FILE="openthread-config-cc13x2_26x2-ftd.h" \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/ti/thread/cc13x2_26x2/libopenthread_ftd/config \
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
    -DOPENTHREAD_FTD=1 \
    -DOPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=1 \
    -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="openthread-core-cc13x2_26x2-config.h" \
    -DOPENTHREAD_CONFIG_FILE="openthread-config-cc13x2_26x2-ftd.h" \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/ti/thread/cc13x2_26x2/libopenthread_ftd/config \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/src/core \
    -I$COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR$/source/third_party/openthread/src \
    --cpu=Cortex-M4F

all: $(NAME).a

src_lib_spinel_spinel.obj: OPENTHREAD_MAKE_PATH/src/lib/spinel/spinel.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

src_ncp_changed_props_set.obj: OPENTHREAD_MAKE_PATH/src/ncp/changed_props_set.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< -o $@

src_ncp_example_vendor_hook.obj: OPENTHREAD_MAKE_PATH/src/ncp/example_vendor_hook.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< -o $@

src_ncp_ncp_base.obj: OPENTHREAD_MAKE_PATH/src/ncp/ncp_base.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< -o $@

src_ncp_ncp_base_dispatcher.obj: OPENTHREAD_MAKE_PATH/src/ncp/ncp_base_dispatcher.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< -o $@

src_ncp_ncp_base_ftd.obj: OPENTHREAD_MAKE_PATH/src/ncp/ncp_base_ftd.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< -o $@

src_ncp_ncp_base_mtd.obj: OPENTHREAD_MAKE_PATH/src/ncp/ncp_base_mtd.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< -o $@

src_ncp_ncp_base_radio.obj: OPENTHREAD_MAKE_PATH/src/ncp/ncp_base_radio.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< -o $@

src_ncp_ncp_spi.obj: OPENTHREAD_MAKE_PATH/src/ncp/ncp_spi.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< -o $@

src_ncp_ncp_uart.obj: OPENTHREAD_MAKE_PATH/src/ncp/ncp_uart.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< -o $@

src_lib_hdlc_hdlc.obj: OPENTHREAD_MAKE_PATH/src/lib/hdlc/hdlc.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< -o $@

src_lib_spinel_spinel_buffer.obj: OPENTHREAD_MAKE_PATH/src/lib/spinel/spinel_buffer.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< -o $@

src_lib_spinel_spinel_decoder.obj: OPENTHREAD_MAKE_PATH/src/lib/spinel/spinel_decoder.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< -o $@

src_lib_spinel_spinel_encoder.obj: OPENTHREAD_MAKE_PATH/src/lib/spinel/spinel_encoder.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< -o $@

$(NAME).a: $(OBJECTS)
	@ echo creating library $@
	@ $(AR) --create $@ $^

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).a > $(DEVNULL) 2>&1
