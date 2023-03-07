#
#  Copyright (c) 2012, Texas Instruments Incorporated
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#  *  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#
#  *  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#  *  Neither the name of Texas Instruments Incorporated nor the names of
#     its contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

#
#  ======== ti.uia.linux/Makefile ========
#

ifneq (clean,$(MAKECMDGOALS))
ifeq (,$(wildcard lib))
    $(shell mkdir -p lib)
endif
endif

IPC_INSTALL_DIR ?= /db/vtree/library/trees/ipc/ipc-h27/exports/ipc_1_24_02_27/packages
SYSLINK_INSTALL_DIR ?= /db/atree/library/trees/sl/sl-c20/exports/syslink_2_10_03_20/packages

CC = /db/toolsrc/library/tools/vendors/cs/arm/arm-2009q1-203/bin/arm-none-linux-gnueabi-gcc
CPPFLAGS = -O2 -g -Wall -fno-strict-aliasing -I. -I../runtime -I../../.. -I$(SYSLINK_INSTALL_DIR) -I$(IPC_INSTALL_DIR)

OUTPUT_OPTION = -o lib/$@

AR = $(CROSS_COMPILE)ar

servicemgr_srcs = ServiceMgr.c
loggersm_srcs = LoggerSM.c

RANLIB = /db/toolsrc/library/tools/vendors/cs/arm/arm-2009q1-203/bin/arm-none-linux-gnueabi-ranlib

all: lib/servicemgr.a lib/loggersm.a

lib/servicemgr.a: $(patsubst %.c,%.o,$(servicemgr_srcs))
	@echo "#"
	@echo "# Making $@ ..."
	$(AR) -cr $@ $(addprefix lib/,$^)
	$(RANLIB) $@

lib/loggersm.a: $(patsubst %.c,%.o,$(loggersm_srcs))
	@echo "#"
	@echo "# Making $@ ..."
	$(AR) -cr $@ $(addprefix lib/,$^)
	$(RANLIB) $@

clean::
	rm -f lib/*
