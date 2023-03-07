#!/bin/bash
# Copyright (c) 2020, Linaro. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

# Exit on error
set -e

# Flash with JLinkExe
FILE=flash.jlink
if test -f "$FILE"; then
    rm $FILE
fi
echo "loadfile tfm_s.hex
loadfile tfm_ns.hex
exit" >> $FILE
jlinkexe -device lpc55s69 -if swd -speed 2000 -autoconnect 1 -commanderscript $FILE
