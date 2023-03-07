#
# Set location of various cgtools
#
# These variables can be set here or on the command line.  Paths must not
# have spaces.
#
# The various *_ARMCOMPILER variables, in addition to pointing to
# their respective locations, also serve as "switches" for disabling a build
# using those cgtools. To disable a build using a specific cgtool, either set
# the cgtool's variable to empty or delete/comment-out its definition:
#     CCS_ARMCOMPILER ?=
# or
#     #CCS_ARMCOMPILER ?= ...
#
# If a cgtool's *_ARMCOMPILER variable is set (non-empty), various sub-makes
# in the installation will attempt to build with that cgtool.  This means
# that if multiple *_ARMCOMPILER cgtool variables are set, the sub-makes
# will build using each non-empty *_ARMCOMPILER cgtool.
#

XDC_INSTALL_DIR        ?= /home/username/ti/xdctools_3_62_01_15_core
SYSCONFIG_TOOL         ?= /home/username/ti/ccs1100/ccs/utils/sysconfig_1.10.0/sysconfig_cli.sh

FREERTOS_INSTALL_DIR   ?= /home/username/FreeRTOSv202104.00

CCS_ARMCOMPILER        ?= /home/username/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS
TICLANG_ARMCOMPILER    ?= /home/username/ti/ccs1100/ccs/tools/compiler/ti-cgt-armllvm_1.3.0.LTS
GCC_ARMCOMPILER        ?= /home/username/ti/ccs1100/ccs/tools/compiler/9.2019.q4.major

# The IAR compiler is not supported on Linux
# IAR_ARMCOMPILER      ?=

# For Linux
RM     = rm -f
RMDIR  = rm -rf
DEVNULL = /dev/null
ECHOBLANKLINE = echo
