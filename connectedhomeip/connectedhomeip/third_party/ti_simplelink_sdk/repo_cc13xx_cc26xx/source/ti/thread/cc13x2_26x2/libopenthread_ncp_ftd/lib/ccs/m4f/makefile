SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR ?= $(abspath ../../../../..)

include $(SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR)/imports.mak

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
AR = "$(CCS_ARMCOMPILER)/bin/armar"

OBJECTS = src_ncp_changed_props_set.obj src_ncp_example_vendor_hook.obj src_ncp_ncp_base.obj src_ncp_ncp_base_dispatcher.obj src_ncp_ncp_base_ftd.obj src_ncp_ncp_base_mtd.obj src_ncp_ncp_base_radio.obj src_ncp_ncp_spi.obj src_ncp_ncp_uart.obj src_lib_hdlc_hdlc.obj src_lib_spinel_spinel.obj src_lib_spinel_spinel_buffer.obj src_lib_spinel_spinel_decoder.obj src_lib_spinel_spinel_encoder.obj
NAME = libopenthread_ncp_ftd

CFLAGS = -I../../.. \
    -O4 \
    --opt_for_speed=0 \
    -mv7M4 \
    --abi=eabi \
    -me \
    --c99 \
    --gcc \
    --float_operations_allowed=none \
    --printf_support=nofloat \
    --gen_func_subsections=on \
    --embedded_constants=on \
    --wchar_t=16 \
    --enum_type=packed \
    --gen_data_subsections=on \
    --common=on \
    --plain_char=unsigned \
    --diag_suppress=23000 \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/examples/platforms \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/third_party/mbedtls \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/third_party/mbedtls/repo/include \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/include \
    -DDeviceFamily_CC13X2_CC26X2 \
    -DHAVE_CONFIG_H \
    -D_BSD_SOURCE=1 \
    -D_DEFAULT_SOURCE=1 \
    -DMBEDTLS_CONFIG_FILE='"mbedtls-config-cc13x2_26x2.h"' \
    -I${WorkspaceDirPath}/libmbedcrypto_cc13x2_26x2_ccs/config \
    -DOPENTHREAD_FTD=1 \
    -DOPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=1 \
    -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE='"openthread-core-cc13x2_26x2-config.h"' \
    -DOPENTHREAD_CONFIG_FILE='"openthread-config-cc13x2_26x2-ftd.h"' \
    -I${WorkspaceDirPath}/libopenthread_ftd_cc13x2_26x2_ccs/config \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/src/core \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/src \
    --silicon_version=7M4 \
    --code_state=16 \
    --little_endian \
    --display_error_number \
    --diag_warning=255 \
    --diag_wrap=off \
    --float_support=FPv4SPD16 \
    --symdebug:dwarf \
    "-I$(CCS_ARMCOMPILER)/include"

CPPFLAGS = -I../../.. \
    -O4 \
    --opt_for_speed=0 \
    -mv7M4 \
    --abi=eabi \
    -me \
    --c99 \
    --gcc \
    --float_operations_allowed=none \
    --printf_support=nofloat \
    --gen_func_subsections=on \
    --embedded_constants=on \
    --wchar_t=16 \
    --enum_type=packed \
    --gen_data_subsections=on \
    --common=on \
    --plain_char=unsigned \
    --diag_suppress=23000 \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/examples/platforms \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/third_party/mbedtls \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/third_party/mbedtls/repo/include \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/include \
    -DDeviceFamily_CC13X2_CC26X2 \
    -DHAVE_CONFIG_H \
    -D_BSD_SOURCE=1 \
    -D_DEFAULT_SOURCE=1 \
    -DMBEDTLS_CONFIG_FILE='"mbedtls-config-cc13x2_26x2.h"' \
    -I${WorkspaceDirPath}/libmbedcrypto_cc13x2_26x2_ccs/config \
    -DOPENTHREAD_FTD=1 \
    -DOPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=1 \
    -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE='"openthread-core-cc13x2_26x2-config.h"' \
    -DOPENTHREAD_CONFIG_FILE='"openthread-config-cc13x2_26x2-ftd.h"' \
    -I${WorkspaceDirPath}/libopenthread_ftd_cc13x2_26x2_ccs/config \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/src/core \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/src \
    --silicon_version=7M4 \
    --code_state=16 \
    --little_endian \
    --display_error_number \
    --diag_warning=255 \
    --diag_wrap=off \
    --float_support=FPv4SPD16 \
    --symdebug:dwarf \
    "-I$(CCS_ARMCOMPILER)/include"

all: $(NAME).a

src_lib_spinel_spinel.obj: OPENTHREAD_MAKE_PATH/src/lib/spinel/spinel.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

src_ncp_changed_props_set.obj: OPENTHREAD_MAKE_PATH/src/ncp/changed_props_set.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< --output_file=$@

src_ncp_example_vendor_hook.obj: OPENTHREAD_MAKE_PATH/src/ncp/example_vendor_hook.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< --output_file=$@

src_ncp_ncp_base.obj: OPENTHREAD_MAKE_PATH/src/ncp/ncp_base.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< --output_file=$@

src_ncp_ncp_base_dispatcher.obj: OPENTHREAD_MAKE_PATH/src/ncp/ncp_base_dispatcher.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< --output_file=$@

src_ncp_ncp_base_ftd.obj: OPENTHREAD_MAKE_PATH/src/ncp/ncp_base_ftd.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< --output_file=$@

src_ncp_ncp_base_mtd.obj: OPENTHREAD_MAKE_PATH/src/ncp/ncp_base_mtd.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< --output_file=$@

src_ncp_ncp_base_radio.obj: OPENTHREAD_MAKE_PATH/src/ncp/ncp_base_radio.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< --output_file=$@

src_ncp_ncp_spi.obj: OPENTHREAD_MAKE_PATH/src/ncp/ncp_spi.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< --output_file=$@

src_ncp_ncp_uart.obj: OPENTHREAD_MAKE_PATH/src/ncp/ncp_uart.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< --output_file=$@

src_lib_hdlc_hdlc.obj: OPENTHREAD_MAKE_PATH/src/lib/hdlc/hdlc.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< --output_file=$@

src_lib_spinel_spinel_buffer.obj: OPENTHREAD_MAKE_PATH/src/lib/spinel/spinel_buffer.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< --output_file=$@

src_lib_spinel_spinel_decoder.obj: OPENTHREAD_MAKE_PATH/src/lib/spinel/spinel_decoder.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< --output_file=$@

src_lib_spinel_spinel_encoder.obj: OPENTHREAD_MAKE_PATH/src/lib/spinel/spinel_encoder.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< --output_file=$@

$(NAME).a: $(OBJECTS)
	@ echo creating library $@
	@ $(AR) r $@ $^

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).a > $(DEVNULL) 2>&1
