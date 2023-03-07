SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR ?= $(abspath ../../../../..)

include $(SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR)/imports.mak

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
AR = "$(TICLANG_ARMCOMPILER)/bin/tiarmar"

OBJECTS = src_cli_cli.obj src_cli_cli_coap.obj src_cli_cli_coap_secure.obj src_cli_cli_commissioner.obj src_cli_cli_console.obj src_cli_cli_dataset.obj src_cli_cli_joiner.obj src_cli_cli_server.obj src_cli_cli_uart.obj src_cli_cli_udp.obj
NAME = libopenthread_cli_ftd

CFLAGS = -I../../.. \
    -Oz \
    -g \
    -gdwarf-3 \
    -gstrict-dwarf \
    -Wall \
    -Wno-ti-macros \
    -ffunction-sections \
    -Wextra \
    -Wshadow \
    -Werror \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/examples/platforms \
    -DHAVE_CONFIG_H -D_BSD_SOURCE=1 -D_DEFAULT_SOURCE=1 \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/third_party/mbedtls \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/third_party/mbedtls/repo/include \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/include \
    -DMBEDTLS_CONFIG_FILE='"mbedtls-config-cc13x2_26x2.h"' \
    -I${WorkspaceDirPath}/libmbedcrypto_cc13x2_26x2_ticlang/config \
    -DDeviceFamily_CC13X2_CC26X2 \
    -DOPENTHREAD_FTD=1 \
    -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE='"openthread-core-cc13x2_26x2-config.h"' \
    -DOPENTHREAD_CONFIG_FILE='"openthread-config-cc13x2_26x2-ftd.h"' \
    -I${WorkspaceDirPath}/libopenthread_ftd_cc13x2_26x2_ticlang/config \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/src/core \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/src \
    -mcpu=cortex-m4 \
    -march=armv7e-m \
    -mthumb \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16

CPPFLAGS = -I../../.. \
    -Oz \
    -g \
    -gdwarf-3 \
    -gstrict-dwarf \
    -Wall \
    -Wno-ti-macros \
    -ffunction-sections \
    -Wextra \
    -Wshadow \
    -Werror \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/examples/platforms \
    -DHAVE_CONFIG_H -D_BSD_SOURCE=1 -D_DEFAULT_SOURCE=1 \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/third_party/mbedtls \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/third_party/mbedtls/repo/include \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/include \
    -DMBEDTLS_CONFIG_FILE='"mbedtls-config-cc13x2_26x2.h"' \
    -I${WorkspaceDirPath}/libmbedcrypto_cc13x2_26x2_ticlang/config \
    -DDeviceFamily_CC13X2_CC26X2 \
    -DOPENTHREAD_FTD=1 \
    -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE='"openthread-core-cc13x2_26x2-config.h"' \
    -DOPENTHREAD_CONFIG_FILE='"openthread-config-cc13x2_26x2-ftd.h"' \
    -I${WorkspaceDirPath}/libopenthread_ftd_cc13x2_26x2_ticlang/config \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/src/core \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/src \
    -mcpu=cortex-m4 \
    -march=armv7e-m \
    -mthumb \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16

all: $(NAME).a

src_cli_cli.obj: OPENTHREAD_MAKE_PATH/src/cli/cli.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) -c $< -o $@

src_cli_cli_coap.obj: OPENTHREAD_MAKE_PATH/src/cli/cli_coap.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) -c $< -o $@

src_cli_cli_coap_secure.obj: OPENTHREAD_MAKE_PATH/src/cli/cli_coap_secure.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) -c $< -o $@

src_cli_cli_commissioner.obj: OPENTHREAD_MAKE_PATH/src/cli/cli_commissioner.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) -c $< -o $@

src_cli_cli_console.obj: OPENTHREAD_MAKE_PATH/src/cli/cli_console.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) -c $< -o $@

src_cli_cli_dataset.obj: OPENTHREAD_MAKE_PATH/src/cli/cli_dataset.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) -c $< -o $@

src_cli_cli_joiner.obj: OPENTHREAD_MAKE_PATH/src/cli/cli_joiner.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) -c $< -o $@

src_cli_cli_server.obj: OPENTHREAD_MAKE_PATH/src/cli/cli_server.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) -c $< -o $@

src_cli_cli_uart.obj: OPENTHREAD_MAKE_PATH/src/cli/cli_uart.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) -c $< -o $@

src_cli_cli_udp.obj: OPENTHREAD_MAKE_PATH/src/cli/cli_udp.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) -c $< -o $@

$(NAME).a: $(OBJECTS)
	@ echo creating library $@
	@ $(AR) r $@ $^

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).a > $(DEVNULL) 2>&1
