#!/bin/bash
#
# Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
# an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
#
# This software, including source code, documentation and related
# materials ("Software") is owned by Cypress Semiconductor Corporation
# or one of its affiliates ("Cypress") and is protected by and subject to
# worldwide patent protection (United States and foreign),
# United States copyright laws and international treaty provisions.
# Therefore, you may use this Software only as provided in the license
# agreement accompanying the software package from which you
# obtained this Software ("EULA").
# If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
# non-transferable license to copy, modify, and compile the Software
# source code solely for use in connection with Cypress's
# integrated circuit products.  Any reproduction, modification, translation,
# compilation, or representation of this Software except as specified
# above is prohibited without the express written permission of Cypress.
#
# Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
# reserves the right to make changes to the Software without notice. Cypress
# does not assume any liability arising out of the application or use of the
# Software or any product or circuit described in the Software. Cypress does
# not authorize its products for use in any products where a malfunction or
# failure of the Cypress product may reasonably be expected to result in
# significant property damage, injury or death ("High Risk Product"). By
# including Cypress's product in a High Risk Product, the manufacturer
# of such system or application assumes all risk of such use and in doing
# so agrees to indemnify Cypress against all liability.
#
(set -o igncr) 2>/dev/null && set -o igncr; # this comment is required
set -e

#######################################################################################################################
# This script performs pre-build operations to create *.ld file.
#
# usage:
# 	bt_pre_build.bash 	--shell=<modus shell path>
#						--scripts=<wiced scripts path>
#						--def=<ld predefines>
#						--patch=<patch elf file>
#						--ld=<ld script file>
#						--direct_load
#						--overlay=<overlay ld script snippet file>
#						--verbose
#
#######################################################################################################################

USAGE="(-s=|--shell=)<shell path> (-w=|--scripts=)<wiced scripts path> (-f=|--defs=)<linker defines> (-p=|--patch=)<patch elf> (-l=|--ld=)<linker script output> (-o=|--overlay=)<overlay snippet> (-d|--direct)"
if [[ $# -eq 0 ]]; then
	echo "usage: $0 $USAGE"
	exit 1
fi

for i in "$@"
do
	case $i in
		-s=*|--shell=*)
			CYMODUSSHELL="${i#*=}"
			shift
			;;
		-w=*|--scripts=*)
			CYWICEDSCRIPTS="${i#*=}"
			shift
			;;
		-f=*|--defs=*)
			CY_APP_LD_DEFS="${i#*=}"
			shift
			;;
		-p=*|--patch=*)
			CY_APP_PATCH="${i#*=}"
			shift
			;;
		-l=*|--ld=*)
			CY_APP_LD="${i#*=}"
			shift
			;;
		-o=*|--overlay=*)
			CY_APP_OVERLAY="${i#*=}"
			shift
			;;
		-d|--direct)
			CY_APP_DIRECT_LOAD=1
			shift
			;;
		-v|--verbose)
			VERBOSE=1
			shift
			;;
		-h|--help)
			HELP=1
			echo "usage: $0 $USAGE"
			exit 1
			;;
		*)
			echo "bad parameter $i"
			echo "usage: $0 $USAGE"
			echo "failed to generate $CY_APP_LD"
			exit 1
			;;
	esac
done

if [ "$VERBOSE" != "" ]; then
	echo Script: bt_pre_build
	echo 1: CYMODUSSHELL    : $CYMODUSSHELL
	echo 2: CYWICEDSCRIPTS  : $CYWICEDSCRIPTS
	echo 3: CY_APP_LD_DEFS  : $CY_APP_LD_DEFS
	echo 4: CY_APP_PATCH    : $CY_APP_PATCH
	echo 5: CY_APP_LD       : $CY_APP_LD
	echo 6: DIRECT_LOAD     : $CY_APP_DIRECT_LOAD
	echo 7: OVERLAY         : $CY_APP_OVERLAY
fi

# if *.ld exists, don't make a new one
# we will always run this until we can have 'make clean' that removes *.ld
if [ -e "$CY_APP_LD" ]; then
    echo "$CY_APP_LD already present, use clean if it needs updating"
    exit 0
fi

set +e

# set up some tools that may be native and not modus-shell
CY_TOOL_PERL=perl
if ! type "$CY_TOOL_PERL" &> /dev/null; then
CY_TOOL_PERL=$CYMODUSSHELL/bin/perl
fi

#create ld file
"$CY_TOOL_PERL" -I "$CYWICEDSCRIPTS" "$CYWICEDSCRIPTS/wiced-gen-ld.pl" $CY_APP_LD_DEFS DIRECT_LOAD=$CY_APP_DIRECT_LOAD "$CY_APP_PATCH" "$CY_APP_OVERLAY" out="$CY_APP_LD"
if [ $? -eq 0 ]; then
   echo "generated $CY_APP_LD"
else
   echo "failed to generate $CY_APP_LD"
   exit 1
fi
