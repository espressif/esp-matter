#===============================================================================
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#===============================================================================

#!/bin/bash
PROJ_ROOT=$(pwd)
MBEDTLS_ROOT_DIR=$PROJ_ROOT/mbedtls
HOST_PROJ_ROOT=$PROJ_ROOT/host
MBEDTLS_ALT_API=$PROJ_ROOT/shared/include/mbedtls
MBEDTLS_PAL_INCDIRS+="$PROJ_ROOT/shared/include $PROJ_ROOT/shared/hw/include $PROJ_ROOT/shared/include/pal $PROJ_ROOT/shared/include/proj/cc3x $PROJ_ROOT/host/src/tests/common"
MBEDTLS_CFLAGS=" -DUSE_MBEDTLS_CRYPTOCELL -I$MBEDTLS_ALT_API -I$PROJ_ROOT/host/include -I$PROJ_ROOT/shared/include/crypto_api/cc3x/ -I$PROJ_ROOT/shared/include/crypto_api/"
#=========================== BUILD MBEDTLS LIBRARIES ==============================


CORTEX=$ARM_CPU

if [ "$CROSS_COMPILE" == "armcc" ]; then
	TEE_OS=freertos
elif [ "$CROSS_COMPILE" == "arm-xilinx-linux-gnueabi-" ]; then
	TEE_OS=linux
elif [ "$CROSS_COMPILE" == "arm-none-eabi-" ]; then
	if [ "$CORTEX" == "cortex-m33" ] || [ "$CORTEX" == "cortex-m3" ]; then
		TEE_OS=freertos
	else
		TEE_OS=no_os
	fi
elif [ "$CROSS_COMPILE" == "armclang" ]; then
	TEE_OS=freertos
fi

# Set mbedtls_cflags according to the target's operating system (TEE_OS)
if [ "$TEE_OS" == "freertos" ]; then
    MBEDTLS_LDFLAGS+="-L$PROJ_ROOT/host/lib/libcc_312.a "
    MBEDTLS_CFLAGS+=" -DMBEDTLS_CONFIG_FILE='<config-cc312-mps2-freertos.h>' "
    MBEDTLS_CFLAGS+=" -I$KERNEL_DIR/OS/FreeRTOS/Source/include/"
    MBEDTLS_PAL_INCDIRS+=" $PROJ_ROOT/shared/include/pal/freertos/"

    if [ "$CROSS_COMPILE" == "armcc" ]; then
        MBEDTLS_CFLAGS+=" -DARMCM3 "
        MBEDTLS_CFLAGS+=" -I$KERNEL_DIR/OS/FreeRTOS/Source/portable/ARMCC/ARM_CM3/ "
        MBEDTLS_CFLAGS+=" --cpu=cortex-m3 "
    elif [ "$CROSS_COMPILE" == "arm-none-eabi-" ]; then
        if [ "$CORTEX" == "cortex-m3" ]; then
            MBEDTLS_CFLAGS +=" -mcpu=cortex-m3 "
            MBEDTLS_CFLAGS+=" -DARMCM3 "
            MBEDTLS_CFLAGS+=" -I$KERNEL_DIR/OS/FreeRTOS/Source/portable/GCC/ARM_CM3/ "
        elif [ "$CORTEX" == "cortex-m33" ]; then
            MBEDTLS_CFLAGS+=" -march=armv8-m.main "
            MBEDTLS_CFLAGS+=" -mcmse "
            MBEDTLS_CFLAGS+=" -DSSE_200 "
            MBEDTLS_CFLAGS+=" -I$KERNEL_DIR/OS/FreeRTOS/Source/portable/GCC/ARM_CM33/ "
        fi
    elif [ "$CROSS_COMPILE" == "armclang" ]; then
        if [ "$CORTEX" == "cortex-m3" ]; then
            MBEDTLS_CFLAGS+=" --target=arm-arm-none-eabi -mcpu=cortex-m3 "
            MBEDTLS_CFLAGS+=" -mlittle-endian -xc "
            MBEDTLS_CFLAGS+=" -DARMCM3 "
            MBEDTLS_CFLAGS+=" -I$KERNEL_DIR/OS/FreeRTOS/Source/portable/ARMCLANG/ARM_CM3/ "
        elif [ "$CORTEX" == "cortex-m33" ]; then
            MBEDTLS_CFLAGS+=" --target=arm-arm-none-eabi -march=armv8-m.main "
            MBEDTLS_CFLAGS+=" -mlittle-endian -xc "
            MBEDTLS_CFLAGS+=" -mcmse "
            MBEDTLS_CFLAGS+=" -mfpu=none "
            MBEDTLS_CFLAGS+=" -DSSE_200 "
            MBEDTLS_CFLAGS+=" -I$KERNEL_DIR/OS/FreeRTOS/Source/portable/ARMCLANG/ARM_CM33/ "
        fi
    fi
fi

if [ "$TEE_OS" == "no_os" ]; then
    MBEDTLS_CFLAGS+=" -DMBEDTLS_CONFIG_FILE='<config-cc312-mps2-no-os.h>' "
    MBEDTLS_PAL_INCDIRS+=" $PROJ_ROOT/shared/include/pal/no_os/"
fi

if [ "$TEE_OS" == "linux" ]; then
    MBEDTLS_CFLAGS+=" -DMBEDTLS_CONFIG_FILE='<config-cc312.h>' "
fi

if [ "$CROSS_COMPILE" == "armcc" ]; then
    export CC="armcc"
    export AR="armar"
    MBEDTLS_CFLAGS+=" --thumb "

    # Suppresses license management warning
    MBEDTLS_CFLAGS+=" --diag_suppress 9931 "

    MBEDTLS_LDFLAGS+="-L$PROJ_ROOT/host/lib/libcc_312.a "
    MBEDTLS_TESTS_SRCFILES+="$PROJ_ROOT/host/src/tests/common/tests_hw_access_freertos.c $PROJ_ROOT/host/src/tests/common/tests_phys_map_freertos.c"

	if [ -d $MBEDTLS_ROOT_DIR ]; then
        #surpress warning levels in library makefile as arm-compiler-5 armcc does not seem to supoort it.
	    sed -i '/WARNING_CFLAGS /d' $MBEDTLS_ROOT_DIR/library/Makefile

	    #change debug flag -g3 to -g to comply with arm-compiler-5 armcc flags
	    sed -i 's|g3|g| g ' $MBEDTLS_ROOT_DIR/library/Makefile
    fi
elif [ "$CROSS_COMPILE" == "arm-xilinx-linux-gnueabi-" ]; then

    export CC="arm-xilinx-linux-gnueabi-gcc"
    export AR="arm-xilinx-linux-gnueabi-ar"

    MBEDTLS_TESTS_LDFLAGS+="-L../../host/lib -lpal_linux -lcc_312 -lpthread "
    # as we cannot change mbedtls Makefile, we add these libs before mbedtls* in order to pass link
    export LOCAL_LDFLAGS+="-L$PROJ_ROOT/host/lib -L$PROJ_ROOT/mbedtls/library -Wl,--start-group -lcc_312 "
    export LOCAL_LDFLAGS+="-lmbedtls "
    export LOCAL_LDFLAGS+="-lmbedx509 "
    export LOCAL_LDFLAGS+="-lmbedcrypto "
    export LOCAL_LDFLAGS+="-Wl,--end-group "

    MBEDTLS_PAL_INCDIRS+=" $PROJ_ROOT/shared/include/pal/cc_linux/"
    MBEDTLS_TESTS_SRCFILES+="$PROJ_ROOT/host/src/tests/common/tests_hw_access_iot.c $PROJ_ROOT/host/src/tests/common/tests_phys_map.c"

    # make sure mbedTLS library makefile is untouched (from above section)
    if [ -d $MBEDTLS_ROOT_DIR ];
    then
        cd $MBEDTLS_ROOT_DIR
        git checkout -q -- $MBEDTLS_ROOT_DIR/library/Makefile
    fi
elif [ "$CROSS_COMPILE" == "arm-none-eabi-" ]; then
    export CC="arm-none-eabi-gcc"
    export AR="arm-none-eabi-ar"
    MBEDTLS_CFLAGS+=" -mthumb "
    MBEDTLS_PAL_INCDIRS+=" $MBEDTLS_ROOT_DIR/include/mbedtls"
elif [ "$CROSS_COMPILE" == "armclang" ]; then
    export CC="armclang"
    export AR="armar"
    MBEDTLS_CFLAGS+=" -mthumb "
    MBEDTLS_CFLAGS+=" -Wno-license-management "
    MBEDTLS_PAL_INCDIRS+=" $MBEDTLS_ROOT_DIR/include/mbedtls "
else
    export CC="cc"
    export AR="ar"
fi

MBEDTLS_CFLAGS+=-I${MBEDTLS_PAL_INCDIRS// / -I}

if [ "$DEBUG" == "" ]; then
    MBEDTLS_CFLAGS+=" -O2"
fi

function mbedtls_git
{
    cd $PROJ_ROOT
    if [ ! -d $MBEDTLS_ROOT_DIR ];
    then
        echo "cloning mbedtls into $MBEDTLS_ROOT_DIR"
        git clone -b "mbedtls-2.16.2"  https://github.com/ARMmbed/mbedtls.git $MBEDTLS_ROOT_DIR
    else
        echo "rebase mbedtls $MBEDTLS_ROOT_DIR"
        cd $MBEDTLS_ROOT_DIR
        git pull --rebase
    fi
}


function mbedtls_build_lib
{
    cd $MBEDTLS_ROOT_DIR
    echo CFLAGS=$MBEDTLS_CFLAGS
    CFLAGS=$MBEDTLS_CFLAGS make -j8 lib

    if [[ $? -ne 0 ]]; then
        exit 1;
    fi

    if [[ "$CROSS_COMPILE" == "armcc" ]] || [[ "$CROSS_COMPILE" == "arm-none-eabi-" ]] || [[ "$CROSS_COMPILE" == "armclang" ]]; then
        sync
        cd $MBEDTLS_ROOT_DIR
        cp `find library/ -name *.a` $KERNEL_DIR/lib/
    fi
}

#============================== CLEAN MBEDTLS =================================
function mbedtls_clean
{
    cd $PROJ_ROOT/mbedtls
    make clean
}
if [ "$#" == "0" ];
then
    echo "Usage: $0 clone|lib|all"
    exit 1
fi

MODE=$1

echo Running prepare_mbedtls.sh with $MODE
if [ "$MODE" == "clone" ] || [ "$MODE" == "all" ]
then
    mbedtls_git
fi

if [ "$MODE" == "lib" ] || [ "$MODE" == "all" ]
then
    mbedtls_build_lib
fi

if [ "$MODE" == "clean" ]
then
    mbedtls_clean
fi
