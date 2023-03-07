SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR ?= $(abspath ../../../../..)

include $(SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR)/imports.mak

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
AR = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-ar"

OBJECTS = examples_platforms_utils_debug_uart.obj examples_platforms_utils_mac_frame.obj
NAME = libopenthread_platform_utils_mtd

CFLAGS = -I../../.. \
    -g \
    -Os \
    -std=c99 \
    -fno-exceptions \
    -fno-rtti \
    -std=gnu++98 \
    -Wno-c++14-compat \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/examples/platforms \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/third_party/mbedtls \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/third_party/mbedtls/repo/include \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/include \
    -DDeviceFamily_CC13X2_CC26X2 \
    -DHAVE_CONFIG_H \
    -D_BSD_SOURCE=1 \
    -D_DEFAULT_SOURCE=1 \
    -march=armv7e-m \
    -mthumb \
    -mcpu=cortex-m4 \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 \
    -fdata-sections \
    -ffunction-sections \
    -Wall \
    -Wextra \
    -Wshadow \
    -DMBEDTLS_CONFIG_FILE='"mbedtls-config-cc13x2_26x2.h"' \
    -I${WorkspaceDirPath}/libmbedcrypto_cc13x2_26x2_gcc/config \
    -DOPENTHREAD_MTD=1 \
    -DOPENTHREAD_CONFIG_FILE='"openthread-config-cc13x2_26x2-mtd.h"' \
    -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE='"openthread-core-cc13x2_26x2-config.h"' \
    -I${WorkspaceDirPath}/libopenthread_mtd_cc13x2_26x2_gcc/config \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/src/core \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/src \
    -gstrict-dwarf \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include/newlib-nano" \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include"

CPPFLAGS = -I../../.. \
    -g \
    -Os \
    -std=c99 \
    -fno-exceptions \
    -fno-rtti \
    -std=gnu++98 \
    -Wno-c++14-compat \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/examples/platforms \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/third_party/mbedtls \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/third_party/mbedtls/repo/include \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/include \
    -DDeviceFamily_CC13X2_CC26X2 \
    -DHAVE_CONFIG_H \
    -D_BSD_SOURCE=1 \
    -D_DEFAULT_SOURCE=1 \
    -march=armv7e-m \
    -mthumb \
    -std=c++11 \
    -mcpu=cortex-m4 \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 \
    -fdata-sections \
    -ffunction-sections \
    -Wall \
    -Wextra \
    -Wshadow \
    -DMBEDTLS_CONFIG_FILE='"mbedtls-config-cc13x2_26x2.h"' \
    -I${WorkspaceDirPath}/libmbedcrypto_cc13x2_26x2_gcc/config \
    -DOPENTHREAD_MTD=1 \
    -DOPENTHREAD_CONFIG_FILE='"openthread-config-cc13x2_26x2-mtd.h"' \
    -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE='"openthread-core-cc13x2_26x2-config.h"' \
    -I${WorkspaceDirPath}/libopenthread_mtd_cc13x2_26x2_gcc/config \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/src/core \
    -I${COM_TI_SIMPLELINK_CC13XX_CC26XX_SDK_INSTALL_DIR}/source/third_party/openthread/src \
    -gstrict-dwarf \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include/newlib-nano" \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include"

all: $(NAME).a

examples_platforms_utils_debug_uart.obj: OPENTHREAD_MAKE_PATH/examples/platforms/utils/debug_uart.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

examples_platforms_utils_mac_frame.obj: OPENTHREAD_MAKE_PATH/examples/platforms/utils/mac_frame.cpp
	@ echo Building $@
	@ $(CC) $(CPPFLAGS) $< -c -o $@

$(NAME).a: $(OBJECTS)
	@ echo creating library $@
	@ $(AR) r $@ $^

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).a > $(DEVNULL) 2>&1
