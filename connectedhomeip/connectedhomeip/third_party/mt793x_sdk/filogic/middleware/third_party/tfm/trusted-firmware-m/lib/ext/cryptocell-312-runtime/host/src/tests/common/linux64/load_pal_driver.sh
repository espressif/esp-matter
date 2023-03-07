#-------------------------------------------------------------------------------
# Copyright (c) 2001-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#!/bin/sh
LINUX64_DRIVER_NAME="cc_linux_drv"
is_load=$1
is_driver_loaded=`lsmod| grep $LINUX64_DRIVER_NAME| wc -l`
#unload driver in any case
if [[ $is_driver_loaded -ne 0 ]]; then
        echo Unloading driver $LINUX64_DRIVER_NAME
        modprobe -r $LINUX64_DRIVER_NAME
        echo lsmod
        lsmod
fi

if [[ $is_load == "load" ]]; then
        echo Loading driver $LINUX64_DRIVER_NAME
        modprobe $LINUX64_DRIVER_NAME
        echo lsmod
        lsmod
fi
